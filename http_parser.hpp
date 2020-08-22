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

#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <iostream>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <curl.h>
#include "string_util.hpp"
#include "hex_dump.hpp"
#include "http_io.hpp"
#include "jsonxx.h" ///< jsonxx::Settings::Parser = Strict, Assertions = Disabled.
#include "debug_output.hpp"
#include "base64.hpp"
#include "magic_mime.hpp"

using namespace std;

class Http_parser {
private:

    const char http_content_type[24] = R"END(^(.*?);\s(.*)$)END";
    regex content_type_regex{http_content_type, regex_constants::optimize};

    const string file_regex_line1 = R"END(Content-Disposition: form-data; name="(.*?)"; filename="(.*?)")END";
    regex multipart_form_file_l1{file_regex_line1, regex_constants::optimize};

    const string file_regex_line2 = R"END(Content-Type: (.*))END";
    regex multipart_form_file_l2{file_regex_line2, regex_constants::optimize};

    const string data_regex = R"END(Content-Disposition: form-data; name="(.*)")END";
    regex multipart_form_data{data_regex, regex_constants::optimize};

public:
    string boundary;
    string charset;
    string content_type;

    const string cr_nl{"\r\n"};
    const string cr_nl_2{"\r\n\r\n"};
    static const int cr_nl_s = 2;
    static const int cr_nl_2s = 4;

    map<string, vector<string>> get;
    map<string, vector<string>> post;
    map<string, vector<map<string, string>>> file;

    explicit Http_parser(map<string, string> &env_ptr);
    ~Http_parser();
    void *curl;
    string_util su;
    map<string, string> env;
    void parse_raw_content_type();
    void parse_query_string();
    void parse_json();
    void parse_multipart_form();
    void print_all(ostream &o_stream, http_io &io, bool hex_dump = false);
    void print_file(ostream &o_stream, bool hex_dump = false);
    void print_get(ostream &o_stream, bool hex_dump = false);
    void print_post(ostream &o_stream, bool hex_dump = false);

};


#endif //HTTP_PARSER_HPP
