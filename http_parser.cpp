//
/*
    Copyright (c) 2020 Patrick Moffitt

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "http_parser.hpp"

/**
 * A class for parsing the stdin passed to us by the CGI web server.
 *
 * Note that the order of operations is dependent.
 *
 * @param env_ptr The web server CGI shell environment passed into main()
 * and parsed into Http_io::env
 */
Http_parser::Http_parser(map<string, string> &env_ptr) {
    env = env_ptr;
    curl = curl_easy_init();
    parse_raw_content_type();
    parse_query_string();
    parse_multipart_form();
    parse_json();
}

Http_parser::~Http_parser() {
    curl_easy_cleanup(curl);
}

/**
 * Sets Http_parser::content_type, Http_parser::boundary, Http_parser::charset from
 * values found in the raw HTTP Content-Type header.
 */
void Http_parser::parse_raw_content_type() {
    content_type = env["content_type"];
    const string mime_type{"multipart/form-data"};
    if (content_type.compare(0, mime_type.length(), mime_type) != 0)
        return; ///< No boundary or charset to parse.

    smatch match;
    regex_search(env["content_type"], match, content_type_regex);
    if (not match.empty()) {
        content_type = match[1];
        auto pair = su.explode(match[2].str(), "=");
        if (!pair.empty()) {
            if ( pair.at(0) == "boundary" ) {
                boundary = "--" + pair.at(1) + cr_nl;
            }
            if ( pair.at(0) == "charset" ) {
                charset = pair.at(1);
            }
        }
    } else {
        DEBUG(env["script_name"].c_str(),
              " parse_raw_content_type() Error, No match found in: (",
              env["content_type"].c_str(), ")");
    }
}

/**
 * Sets Http_parser::get from values found in the raw HTTP query string.
 */
void Http_parser::parse_query_string() {
    if (env["query_string"].empty()) return;
    int result_len;
    char *result = curl_easy_unescape(curl, env["query_string"].c_str(), 0, &result_len);
    string query(result);
    curl_free(result);
    if (query.empty()) return;
    auto pairs = su.explode(&query, "&");
    for (auto p: pairs) {
        auto pair = su.explode(&p, "=");
        if (not pair[0].empty() && not pair[1].empty()) {
            get[pair[0]].emplace_back(pair[1]);
        }
    }
}

/**
 * Parse an HTTP message body in JSON format.
 *
 * Stores key:value pairs in Http_parser::post and Http_parser::file objects.
 */
void Http_parser::parse_json() {
    if (content_type != "application/json"
        or env["body"].empty()) return;
    using namespace jsonxx;
    Object json; ///< jsonxx::Settings; Parser = Strict, Assertions = Disabled.
    bool r = json.parse(env["body"]);
    if (not r) {
        DEBUG("JSON parser failed.");
        return;
    }
    if (not json.has<Array>("body")) {
        DEBUG("JSON object is missing the \"body\" array.");
        return;
    }
    auto& body = json.get<Array>("body");
    for (int i=0; i < body.size(); i++) {
        if (not body.has<Object>(i)) {
            DEBUG("JSON object has an empty \"body\" array.");
            break;
        }
        auto& item = body.get<Object>(i);
        // Put string values in Http_parser::post
        if (item.size() == 2 and item.has<String>("value")) {
            string name;
            for (auto &pair: item.kv_map()) {
                if (pair.first == "name" and pair.second->is<String>()) {
                    name = pair.second->get<String>();
                    auto search = post.find(name);
                    if (search == post.end()) {
                        post[name] = vector<string>{};
                    }
                }
                if (pair.first == "value" and not name.empty()
                    and pair.second->is<String>()) {
                    auto& value = pair.second->get<String>();
                    post[name].emplace_back(value);
                }
            }
        }
        // Put array values in Http_parser::post
        if (item.size() == 2 and item.has<Array>("value")) {
            auto it = item.kv_map().begin();
            string name;
            if (it->first == "name" and it->second->is<String>()) {
                name = it->second->get<String>();
                auto search = post.find(name);
                if (search == post.end()) {
                    post[name] = vector<string>{};
                }
            }
            it++;
            if (not name.empty() and it->first == "value"
                                 and it->second->is<Array>()) {

                auto a = it->second->get<Array>().values();
                for (auto & j : a) {
                    auto &value = j->get<String>();
                    post[name].emplace_back(value);
                }
            }
        }
        // Emplace into Http_parser::file
        if (item.size() == 4 and item.has<String>("name")
                             and item.has<Number>("frames")
                             and item.has<String>("content_type")
                             and item.has<String>("value")) {
            auto& key = item.get<String>("name");
            auto& frames = item.get<Number>("frames");
            auto& mime = item.get<String>("content_type");
            auto& value = item.get<String>("value");

            string ext{".jpg"}; // Single frame JPEG data.
            if (frames > 0) ext = ".avi"; // Multiple frame MJPEG data.

            string filename = to_string(time(nullptr)) + ext;
            file[key].emplace_back(map<string, string>{
                    {"filename", filename},
                    {"frames",   to_string(int(frames))},
                    {"type",     mime},
                    {"value",    move(value)}
            });
        }
    }
}

/**
 * Parse an HTTP message body in multipart/form-data format.
 *
 * Stores key:value pairs in Http_parser::post and file objects
 * in Http_parser::file.
 */
void Http_parser::parse_multipart_form() {
    if (content_type != "multipart/form-data"
        or boundary.empty()
        or env["body"].empty()) return;
    smatch match;
    vector<string> output{};
    string raw_body = env["body"].substr(0, env["body"].rfind("--"));
    su.explode(&raw_body, boundary, &output);
    for (auto s: output) {
        if (s.empty()) continue;
        map<string, string> data;
        if (s.find("filename=") != string::npos) { // Item is a file field.
            int eol_1 = s.find(cr_nl, 0); // end of line (eol).
            int line2_start = eol_1 + cr_nl_s;
            int eol_2 = s.find(cr_nl_2, line2_start); // Extra cr_nl consumes empty line3.
            int line2_length = eol_2 - line2_start;
            int line4_start = eol_2 + cr_nl_2s;
            int eol_4 = s.size() - cr_nl_s;
            int line4_length = eol_4 - line4_start;
            string line1 = s.substr(0, eol_1);
            string line2 = s.substr(line2_start, line2_length);
            string line4 = s.substr(line4_start, line4_length);
            regex_search(line1, match, multipart_form_file_l1);
            string key = match[1];
            string filename{};
            string mime{};
            if (match.size() == 3) filename = match[2];
            regex_search(line2, match, multipart_form_file_l2);
            if (match.size() == 2) mime = match[1];
            if (not filename.empty() and
                not mime.empty() and
                not line4.empty()) {
                file[key].emplace_back(map<string, string>{
                        {"filename", filename},
                        {"type",     mime},
                        {"value",    line4}
                });
            }
        } else { // Item is a data field.
            int eol_1 = s.find(cr_nl_2, 0); // Extra cr_nl consumes empty line 2.
            int line3_start = eol_1 + cr_nl_2s;
            int eol_3 = s.find(cr_nl, line3_start);
            int line3_length = eol_3 - line3_start;
            string line1 = s.substr(0, eol_1);
            string line3 = s.substr(line3_start, line3_length);
            regex_search(s, match, multipart_form_data);
            if (match.size() == 2) {
                auto search = post.find(match[1]);
                if (search != post.end()) {
                    post[match[1]].emplace_back(line3);
                } else {
                    post[match[1]] = vector<string>{line3};
                }
            }
        }
    }
}

/**
 * Dump all the data structures (optionally in hex_dump format) to any stream.
 * Calls Http_io::print_env, Http_parser::print_file, Http_parser::print_post, and
 * Http_parser::print_get.
 *
 * @param o_stream The stream to print the output to; stdout or a file typically.
 * @param io Additional data to print.
 * @param hex_dump whether to print in hex_dump format.
 */
void Http_parser::print_all(ostream &o_stream, http_io &io, bool hex_dump) {

    o_stream << "Locale: "                   << io.locale    << endl;
    o_stream << "TLS enabled: " << boolalpha << io.https     << endl;
    o_stream << "Content type: "             << content_type << endl;
    o_stream << "Multipart form boundary: "  << boundary     << endl;
    o_stream << "Character set: "            << charset      << endl;

    io.print_env(o_stream);
    o_stream << endl;

    print_file(o_stream, hex_dump);
    o_stream << endl;

    print_post(o_stream, hex_dump);
    o_stream << endl;

    print_get(o_stream, hex_dump);
    o_stream << endl;

    if (hex_dump) {
        o_stream << "Body in hex: " << endl;
        hex_dump::string(io.env["body"], o_stream);
    }

    io.header["Status-Message"] = "Http_parser::print_all output.";
    io.respond(dynamic_cast<ostringstream &>(o_stream), 200);
}

/**
 * Dump Http_parser::file (optionally in hex_dump format) to any stream.
 *
 * @param o_stream The stream to print the output to; stdout or a file typically.
 * @param hex_dump whether to print in hex_dump format.
 */
void Http_parser::print_file(ostream &o_stream, bool hex_dump) {
    o_stream << "Http_parser::file" << endl;
    for (const auto& key: file) {
        o_stream << key.first << ": " << endl;
        for (const auto& v: key.second) { // v for vector.
            for (const auto& m: v) {      // m for map.
                if (hex_dump) {
                    o_stream << m.first << ": " << endl;
                    hex_dump::string(m.second, o_stream);
                } else {
                    o_stream << "    " <<  m.first << ": " << m.second;
                }
                o_stream << endl;
            }
        }
        o_stream << endl;
    }
}

/**
 * Dump Http_parser::get (optionally in hex_dump format) to any stream.
 *
 * @param o_stream The stream to print the output to; stdout or a file typically.
 * @param hex_dump whether to print in hex_dump format.
 */
void Http_parser::print_get(ostream &o_stream, bool hex_dump) {
    o_stream << "Http_parser::get" << endl;
    for (const auto& key: get) {
        o_stream << key.first << ": " << endl;
        for (const auto& value: key.second) {
            if (hex_dump) {
                hex_dump::string(value, o_stream);
            } else {
                o_stream << "    " << value << ", ";
            }
        }
        o_stream << endl << endl;
    }
}

/**
 * Dump Http_parser::post (optionally in hex_dump format) to any stream.
 *
 * @param o_stream The stream to print the output to; stdout or a file typically.
 * @param hex_dump whether to print in hex_dump format.
 */
void Http_parser::print_post(ostream &o_stream, bool hex_dump) {
    o_stream << "Http_parser::post" << endl;
    for (const auto& key: post) {
        o_stream << key.first << ": " << endl;
        for (const auto& value: key.second) {
            if (hex_dump) {
                hex_dump::string(value, o_stream);
            } else {
                o_stream << "    " << value << ", ";
            }
        }
        o_stream << endl << endl;
    }
}



