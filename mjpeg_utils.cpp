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

#include "mjpeg_utils.hpp"

/**
 * Splits an AVI|MJPEG file into JPEG images and tests the magic number in each to
 * see if it has a MIME type of image/jpeg. It saves those that do and discards
 * the rest.
 *
 * @param avi input AVI|MJPEG file
 * @param source_file the full path and name of the source file. Used to construct
 * output file names.
 */
void mjpeg_utils::save_avi_frames(const string *avi, const string *source_file) {
    uint32_t l = avi->length();
    uint32_t start{0};
    uint32_t end{0};
    uint16_t frame_no{0};
    ofstream ofs;
    string jpeg{};
    for (uint32_t i=1; i<l; i++) {
        // Find the tail of the image.
        if ((uint8_t) (*avi)[i - 1] == 0xFF and (uint8_t) (*avi)[i] == 0xD9) {
            end = i + 1;
            jpeg = avi->substr(start, end - start);
            auto mime_type = magic_mime::get_mime_type(&jpeg);
            if (mime_type == "image/jpeg") {
                char suffix[10];
                sprintf(suffix, "_%03d.jpg", frame_no);
                string target{*source_file};
                target.replace(target.length() - 4, target.length(), suffix);
                ofs.open(target, ios::binary | ios::out | ios::trunc);
                ofs << jpeg;
                ofs.flush();
                ofs.close();
            }
        }
        // Find the head of the next image.
        if ((uint8_t) (*avi)[i - 1] == 0xFF and (uint8_t) (*avi)[i] == 0xD8) {
            jpeg.clear();
            jpeg.shrink_to_fit();
            start = i - 1;
            frame_no++;
        }
    }
}

/**
 * Remove the extra zeros (0x00) sometimes left after conversion from Base64.
 *
 * If the Base64 encoded string ends with equal sign(s) it will have such a
 * pad when decoded. This version is specific to JPEG files which must end with
 * the sequence 0xFF 0xD9. OpenCV complains of an "overread 8" error if the pad
 * is not removed before conversion from avi to mp4 video.
 *
 * This function also has the effect of removing partial frames from the end of
 * the file; a sort of repair.
 *
 * @param s a Base64 decoded string.
 */
void mjpeg_utils::trim_zero_pad(string &s) {
    uint32_t l = s.length();
    uint32_t eof_idx{l};
    for (uint32_t i=l; i>0; i--) {
        if ((uint8_t) s[i] == 0xd9 and (uint8_t) s[i-1] == 0xff) {
            eof_idx = i + 1;
            break;
        }
    }
    s = s.substr(0, eof_idx);
}