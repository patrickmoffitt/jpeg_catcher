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

#include "hex_dump.hpp"

/*
 * Provides the same out as hexdump(1) or cat file.ext | od -A x -t x1z -v
 *
 * Output goes to std::cout by default but may go to other streams if desired.
 */
void hex_dump::string(const std::string &input, std::ostream &o_stream) {
    if (input.empty()) return;
    unsigned int line_length{};
    unsigned int output_length{};
    int wrap{16}; // Wrap lines at this length.
    uint8_t glyphs[wrap];
    char hexes[wrap];
    o_stream << std::noskipws;
    sprintf(hexes, "%06x ", output_length);
    o_stream << hexes;
    for (int i=0; i<input.length(); i++) {
        sprintf(hexes, "%02x ", (uint8_t) input.at(i));
        o_stream << hexes;
        // ASCII printable range is 21 to 126 inclusive.
        if (input.at(i) > 20 and input.at(i) < 127) {
            glyphs[line_length] = input.at(i);
        } else {
            glyphs[line_length] = '.'; // By convention, print invisibles as dots.
        }
        line_length++;
        if (line_length == wrap or i + 1 == input.length()) {
            unsigned int pad_len = (wrap * 2 + wrap - 1) - (line_length * 2 + line_length - 1);
            std::string pad{};
            if (pad_len > 0) {
                pad = std::string(pad_len, ' ');
            }
            glyphs[line_length] = '\0';
            o_stream << pad << " >" << glyphs << "<" << std::endl;
            output_length += line_length;
            sprintf(hexes, "%06x ", output_length);
            o_stream << hexes;
            line_length = 0;
        }
    }
    o_stream << std::endl;
}

void hex_dump::string(const uint8_t *input, int input_len, std::ostream &o_stream) {
    std::string input_string{};
    for (int i=0; i<input_len; i++) {
        input_string.push_back(input[i]);
    }
    string(input_string, o_stream);
}
