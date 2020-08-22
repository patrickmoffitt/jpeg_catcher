#include <iostream>
#include <thread>
#include <functional>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include "http_io.hpp"
#include "http_parser.hpp"
#include "base64.hpp"
#include "magic_mime.hpp"
#include "opencv_util.hpp"
#include "mjpeg_utils.hpp"

#define JPEG_DEST (string) "/var/www/html/motion_camera/"

using namespace std;
using b64 = Base64;
using mu = mjpeg_utils;

/**
 * A program implementing the CGI for the purpose of storing images and data.
 *
 * @param argc The count of command line arguments passed by the web server to the CGI (us).
 * @param argv The command line argument strings.
 * @param envp The system environment variables as key=value pairs.
 * @return Exit status.
 *
 * @see https://en.wikipedia.org/wiki/Common_Gateway_Interface
 */
int main(int argc, char *argv[], char *envp[]) {
    http_io io;
    io.get_env(envp);

    Http_parser http(io.env);

    ostringstream content;
    content << noskipws;

    //http.print_all(content, io);
    //http.print_post(content);
    //http.print_file(cerr);

    if (http.post.find("battery_percent") != http.post.end()) {
        auto battery_percent = http.post.find("battery_percent")->second.data();
        string target = JPEG_DEST;
        auto dp = opendir(target.data());
        dirent *entry;
        errno = 0;
        while (dp != nullptr and (entry = readdir(dp))) {
            if (errno == 0 and entry != nullptr
                and string(entry->d_name).find("battery_percent_") != string::npos) {
                string file_to_remove{JPEG_DEST};
                file_to_remove.append(entry->d_name);
                remove(file_to_remove.c_str());
            }
        }
        closedir(dp);
        target.append("battery_percent_");
        target.append(*battery_percent);
        target.append(".pwr");
        ofstream ofs;
        ofs.open(target, ios::binary | ios::out | ios::trunc);
        ofs << *battery_percent << endl;
        ofs.close();
    }
    /**
     * A lambda function for writing the Http_parser::files data to file storage.
     * MIME types are checked with magic numbers in the data to confirm a match
     * before writing. Non-matches are skipped.
     *
     * Using a lambda makes it convenient to use a thread for this task.
     */
    auto write_files = [](auto path, auto files) {
        for (const auto& key: files) {
            for (const auto& v: key.second) { // v for vector.
                if (v.find("filename") == v.end()
                    or v.find("frames") == v.end()
                    or v.find("type") == v.end()
                    or v.find("value") == v.end()) {
                    DEBUG("Http_parser::files data are incomplete. Skipping.");
                    continue;
                }
                const string& filename = v.find("filename")->second;
                auto frames = atoi(v.find("frames")->second.c_str());
                string source = path + key.first + "_" + filename;
                string target{source};
                target.replace(target.length() - 3, target.length(), "mp4");
                const string& type = v.find("type")->second;
                const string& value = v.find("value")->second;
                auto decode_size = b64::decode_len(value.data());
                auto value_decoded = b64::decode(value);
                mu::trim_zero_pad(value_decoded);
                auto mime = magic_mime::get_mime_type(&value_decoded);
                if (type == mime) {
                    ofstream ofs;
                    ofs.open(source, ios::binary | ios::out | ios::trunc);
                    if (not ofs.is_open()) {
                        DEBUG("%s %s", "Can't open:", source.c_str());
                    } else {
                        ofs << value_decoded;
                        ofs.flush();
                    }
                    ofs.close();
                } else {
                    DEBUG("%s %s != %s", "MIME type mismatch;", type.c_str(), mime.c_str());
                }
                if (frames == 255) {
                    opencv_util convert;
                    if (convert.mjpeg_to_mp4(source)) {
                        auto status = remove(source.c_str());
                        if (status != 0) DEBUG("%s %s %s, %d", "Failed to delete", source.c_str(), "Error code:", errno);
                    } else {
                        DEBUG("%s %s", "Extracting usable frames from broken-movie:", target.c_str());
                        mu::save_avi_frames(&value_decoded, &target);
                        auto status = remove(source.c_str());
                        if (status != 0) DEBUG("%s %s %s, %d", "Failed to delete", source.c_str(), "Error code:", errno);
                    }
                } else {
                    mu::save_avi_frames(&value_decoded, &target);
                    auto status = remove(source.c_str());
                    if (status != 0) DEBUG("%s %s %s, %d", "Failed to delete", source.c_str(), "Error code:", errno);
                }
            }
        }
    };
    /**
     * The write_files lambda function is called in its own thread to permit the HTTP
     * transaction to conclude (by sending the response) as early as possible.
     */
    thread writer(write_files, JPEG_DEST, http.file);

    io.header["Status-Message"] = io.env["script_name"] + " images caught.";
    io.respond(content);

    writer.join();

    return 0;
}
