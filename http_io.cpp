//
// Created by patrick on 4/9/20.
//

#include "http_io.hpp"

/**
 * Parses the web server CGI shell environment into http_io::env. Puts the HTTP
 * message body into http_io::env["body"].
 *
 * @param env_p The CGI shell environment passed into the third argument to main().
 */
void http_io::get_env(char **env_p) {
    istream_iterator<char> it(cin);
    istream_iterator<char> end;
    string body(it, end);
    if (not body.empty()) env["body"] = move(body);

    int i{0};
    string line, key, value;
    while (env_p[i] != nullptr) {
        line = string(env_p[i]);
        auto center = line.find_first_of('=');
        key = line.substr(0, center);
        value = line.substr(center + 1);
        su.str_tolower(key);
        env[key] = value;
        i++;
    }
    if (env["request_scheme"] == "https") https = true;
}

/**
 * Class to handle basic web server CGI input/output and shell environment tasks.
 */
http_io::http_io() {
    cin  >> noskipws;
    cout << noskipws;
    cerr << noskipws;
    locale = cin.getloc().name();
}

/**
 * Dump http_io::env to a stream.
 *
 * @param o_stream The stream to print the output to; stdout or a file typically.
 */
void http_io::print_env(ostream &o_stream) {
    o_stream << "http_io::env" << endl;
    for (const auto& kv: env) {
        o_stream << kv.first << " = " << kv.second << endl;
    }
}

/**
 * Send an HTTP response to an HTTP query.
 *
 * @param o_stream The stream to print the output to..
 * @param status The HTTP Status code @see http_io::http_status
 * @param mime The HTTP Content-Type
 * @param charset The character set
 */
void http_io::respond(ostringstream &o_stream, int status, string mime, string charset) {
    su.str_toupper(charset);
    cout << "Status: " << status << " " << http_status[status] << endl;
    cout << "Content-Type: " << mime << "; charset=" << charset << endl;
    cout << "Content-Disposition: inline" << endl;
    for (const auto &kv: header) {
        cout << kv.first << ": " << kv.second << endl;
    }
    cout << "Content-Length: " << o_stream.str().length() << endl;
    cout << "Connection: Closed" << endl;
    cout << endl;
    cout << o_stream.str() << endl << endl << endl;
}


