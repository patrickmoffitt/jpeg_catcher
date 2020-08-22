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

#include "magic_mime.hpp"

/**
 * Detect the magic number in a string and return the MIME type.
 *
 * @param input The content of a file.
 * @return The MIME type correspond to the magic number found.
 *
 * @see https://en.wikipedia.org/wiki/Magic_number_(programming)
 */
string magic_mime::get_mime_type(const string *input) {
    string mime{};

    auto cookie = magic_open(MAGIC_MIME_TYPE);
    if (cookie == nullptr) {
        DEBUG("magic_open() failed.");
        mime.clear();
        return mime;
    }

    auto load = magic_load(cookie, nullptr);
    if (load == -1) {
        auto error = magic_error(cookie);
        DEBUG("%s%s", "magic_load() failed. Error: ", error);
        mime.clear();
        return mime;
    }

    mime = magic_buffer(cookie, input->data(), input->length());
    if (mime.empty()) {
        auto error = magic_error(cookie);
        DEBUG("%s%s", "magic_buffer() failed. Error: ", error);
    }
    magic_close(cookie);
    return mime;
}

/**
 * Detect the magic number in a string and return the MIME type.
 *
 * @param input The content of a file.
 * @return The MIME type correspond to the magic number found.
 *
 * @see https://en.wikipedia.org/wiki/Magic_number_(programming)
 */
string magic_mime::get_mime_type(const uint8_t *input, uint32_t input_size) {
    string mime{};

    auto cookie = magic_open(MAGIC_MIME_TYPE);
    if (cookie == nullptr) {
        DEBUG("magic_open() failed.");
        mime.clear();
        return mime;
    }

    auto load = magic_load(cookie, nullptr);
    if (load == -1) {
        auto error = magic_error(cookie);
        DEBUG("%s%s", "magic_load() failed. Error: ", error);
        mime.clear();
        return mime;
    }

    mime = magic_buffer(cookie, input, input_size);
    if (mime.empty()) {
        auto error = magic_error(cookie);
        DEBUG("%s%s", "magic_buffer() failed. Error: ", error);
    }
    magic_close(cookie);
    return mime;
}

/**
 * Detect the magic number in a file and return the MIME type.
 *
 * @param input The path and name of a file.
 * @return The MIME type correspond to the magic number found.
 *
 * @see https://en.wikipedia.org/wiki/Magic_number_(programming)
 */
string magic_mime::get_mime_type(const char *filename) {
    string mime;

    auto cookie = magic_open(MAGIC_MIME_TYPE);
    if (cookie == nullptr) {
        DEBUG("magic_open() failed.");
        mime.clear();
        return mime;
    }

    auto load = magic_load(cookie, nullptr);
    if (load == -1) {
        auto error = magic_error(cookie);
        DEBUG("%s%s", "magic_load() failed. Error: ", error);
        mime.clear();
        return mime;
    }

    mime = magic_file(cookie, filename);
    if (mime.find("cannot open") == 0) {
        DEBUG("magic_file() open failed.");
        mime.clear();
    }

    magic_close(cookie);
    return string(mime);
}