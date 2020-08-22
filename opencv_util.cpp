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

#include "opencv_util.hpp"

/**
 * Uses OpenCV to convert camera MJPEG output to MPEG-4 part 10/H.264 streaming video.
 *
 * @param input_file The source file path/name in MJPEG format (from the camera). See note.
 * @return true on successful conversion to MP4 or false otherwise.
 *
 * @note cv::VideoCapture does not recognize the input_file as MJPEG unless the filename ends in ".avi".
 *
 * @see https://en.wikipedia.org/wiki/Motion_JPEG
 * @see https://en.wikipedia.org/wiki/Advanced_Video_Coding
 * @see https://docs.opencv.org/3.4/d8/dfe/classcv_1_1VideoCapture.html
 * @see https://docs.opencv.org/3.4/dd/d9e/classcv_1_1VideoWriter.html
 * @see https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html
 */
bool opencv_util::mjpeg_to_mp4(string& input_file) {
    string output_file{input_file};
    output_file.replace(output_file.length() - 3, output_file.length(), "mp4");

    Mat src;
    // Use the FFMPEG backend to capture video.
    VideoCapture cap(input_file, CAP_FFMPEG);
    if (!cap.isOpened()) {
        DEBUG("%s %s", "ERROR! Unable to open", input_file.c_str());
        return false;
    }
    // Get one frame from input_file to learn the frame size and encoding type.
    cap >> src;
    if (src.empty()) {
        DEBUG("%s %s", "ERROR! blank frame grabbed from", input_file.c_str());
        return false;
    }
    bool isColor = (src.type() == CV_8UC3);

    VideoWriter writer;
    // Apple's version of the MPEG-4 part 10/H.264 standard http://www.fourcc.org/avc1/
    int codec = VideoWriter::fourcc('a', 'v', 'c', '1'); // Case sensitive.
    double fps = 30.0; // Frame rate for the output video stream.
    writer.open(output_file, codec, fps, src.size(), isColor);
    if (!writer.isOpened()) {
        DEBUG("%s %s %s", "ERROR! Unable to open \"", output_file.c_str(), "\" for writing.");
        return false;
    }
    while (cap.read(src)) {
        writer.write(src);
    }
    writer.release();
    return true;
}

