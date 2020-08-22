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

#include "string_util.hpp"

/**
 * Split a string by a string and place the results in a std::vector<string>.
 *
 * @param input The input string to be split or exploded into parts.
 * @param delimiter The string of characters dividing or delimiting the parts.
 * @param output The list of results.
 */
void string_util::explode(const string *input, string delimiter, vector<string> *output) {
    auto input_length = input->length();
    auto delimiter_length = delimiter.length();
    if (!input_length || !delimiter_length) return;
    if (input_length < delimiter_length) return;
    string::size_type d = input->find_first_of(delimiter);
    if (d == string::npos) return;
    vector<string::size_type> delimiters;
    // Account for no beginning delimiter.
    if (d > 0)  {
        delimiters.push_back(0);
    }
    auto search_from = d + delimiter_length;
    delimiters.push_back(search_from);
    while (d != string::npos && search_from <= input_length) {
        d = input->find(delimiter, search_from);
        search_from = d + delimiter_length;
        if (d <= input_length) delimiters.push_back(search_from);
    }
    // Account for whether there is an ending delimiter on not.
    if (input->substr(input_length - delimiter_length) == delimiter) {
        delimiters.push_back(input_length);
    } else {
        delimiters.push_back(input_length + delimiter_length);
    }

    string::size_type c{0};
    for (auto it = delimiters.begin(); it != delimiters.end(); it++) {
        if (next(it) != delimiters.end()){
            c = *next(it) - *it - delimiter_length;
            output->push_back(input->substr(*it, c));
        }
    }
}

/**
 * Split a string by a string and place the results in a std::vector<string>.
 *
 * @param input The input string to be split or exploded into parts.
 * @param delimiter The string of characters dividing or delimiting the parts.
 * @return output The list of results.
 *
 * @see string_util::explode(const string *input, string delimiter, vector<string> *output)
 */
vector<string> string_util::explode(const string input, string delimiter) {
    vector<string> output;
    const string input_string(input);
    explode(&input_string, delimiter, &output);
    return output;
}

/**
 * Split a string by a string and place the results in a std::vector<string>.
 *
 * @param input The input string to be split or exploded into parts.
 * @param delimiter The string of characters dividing or delimiting the parts.
 * @return output The list of results.
 *
 * @see string_util::explode(const string *input, string delimiter, vector<string> *output)
 */
vector<string> string_util::explode(const string *input, string delimiter) {
    vector<string> output;
    explode(input, delimiter, &output);
    return output;
}

/**
 * Transform the input to lower case letters.
 *
 * @param s the string to be transformed.
 */
void string_util::str_tolower(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return tolower(c); }
    );
}

/**
 * Transform the input to upper case letters.
 *
 * @param s the string to be transformed.
 */
void string_util::str_toupper(string &s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return toupper(c); }
    );
}
