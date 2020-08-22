OpenCV has a large number of package dependencies. To build it you will need to install the following packages.

```shell script
sudo apt-get install libopencv-core-dev libopencv-highgui-dev \
    libopencv-videoio-dev libopencv-imgproc-dev \
    libopencv-imgcodecs-dev libdc1394-22-dev libjpeg-dev \
    libpng-dev libopenlibm-dev zlib1g-dev libdc1394-22-dev \
    libtiff-dev libswresample-dev libavutil-dev libv4l-dev \
    libtbb-dev libavfilter-dev libssh-dev liblzma-dev libzvbi-dev \
    libavcodec-dev libsnappy-dev libopenjp2-7-dev libtwolame-dev \
    libavformat-dev libxvidcore-dev libshine-dev libc6-dev \
    libswscale-dev libgmp-dev libx11-dev libtheora-dev \
    libavdevice-dev libvorbis-dev libogg-dev libpostproc-dev \
    libmp3lame-dev libcrystalhd-dev libx264-dev libspeex-dev \
    libopus-dev libvpx-dev libgsm1-dev libwavpack-dev \
    libwebp-dev libx265-dev libva-dev librtaudio-dev \
    libbz2-dev libopenmpt-dev libusb-dev libgdcm2-dev \
    libxml2-dev libgdal-dev libtiff5-dev \
    libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev
```

These packages support the use of the following linker flags.
```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v -ltbb -lz -lm -lswresample \
                           -lopencv_core -lopencv_videoio -lopencv_highgui -ldc1394  \
                           -lopencv_imgcodecs -lopencv_imgproc -ljpeg -lpng -ltiff \
                           -lavformat -lavcodec -lswscale -lavutil -lavdevice -lavfilter \
                           -lssh -lcrystalhd -llzma -lgsm -lzvbi -lsnappy -lopenjp2 -lv4l2\
                           -lshine -lxvidcore -lwavpack -lwebp -lwebpmux -lx265 -lgmp \
                           -lva -lrt -lX11 -lbz2 -lopenmpt -lusb -lxml2 -lgdcmCommon \
                           -lgdcmDICT -lgdcmDSED -lgdcmIOD -lgdcmjpeg12 -lgdcmjpeg16 \
                           -lgdcmjpeg8 -lgdcmMEXD -lgdcmMSFF -lgdal -lgnutls -lpostproc \
                           -lmp3lame -ltwolame -lvorbis -lvorbisenc -lvorbisfile -lx264 \
                           -lx265 -lspeex -lopus -lvpx -ltheora -logg -lgstvideo-1.0 \
                           -lgstvideo-1.0 -lgstreamer-1.0")
```
The following table provides a cross-reference between the linker flags and the package names and descriptions.

| Linker Symbol    | Package Name                     | Description                                                   |
|:---------------- |:-------------------------------- |:------------------------------------------------------------- |
| opencv_core      | libopencv-core-dev               | Development files for libopencv-core3.2                       |
| opencv_highgui   | libopencv-highgui-dev            | Development files for libopencv-highgui3.2                    |
| opencv_videoio   | libopencv-videoio-dev            | Development files for libopencv-videoio3.2                    |
| opencv_imgproc   | libopencv-imgproc-dev            | Development files for libopencv-imgproc3.2                    |
| opencv_imgcodecs | libopencv-imgcodecs-dev          | Development files for libopencv-imgcodecs3.2                  |
| z                | zlib1g-dev                       | (aka libz-dev) Compression library                            | 
| m                | libopenlibm-dev                  | (aka libm-dev) implementation of C mathematical functions     |
| dc1394           | libdc1394-22-dev                 | High level programming interface for IEEE 1394 cameras        |
| jpeg             | libjpeg-dev                      | Independent JPEG Group's JPEG runtime library                 |
| png              | libpng-dev                       | PNG library - development (version 1.6)                       |
| tiff             | libtiff-dev                      | Tag Image File Format library (TIFF), development files       |
| tbb              | libtbb-dev                       | Parallelism library for C++                                   |
| swresample       | libswresample-dev                | FFmpeg library for audio resampling, rematrixing etc.         |
| avcodec          | libavcodec-dev                   | FFmpeg library with de/encoders for audio/video codecs        |
| avformat         | libavformat-dev                  | FFmpeg library with (de)muxers for multimedia containers      |
| swscale          | libswscale-dev                   | FFmpeg library for image scaling and various conversions      |
| avdevice         | libavdevice-dev                  | FFmpeg library for handling input and output devices          |
| postproc         | libpostproc-dev                  | FFmpeg library for post processing                            |
| avutil           | libavutil-dev                    | FFmpeg library with functions for simplifying programming     |
| avfilter         | libavfilter-dev                  | FFmpeg library containing media filters                       |
| ssh              | libssh-dev                       | tiny C SSH library. Development files (OpenSSL flavor)        |
| lzma             | liblzma-dev                      | XZ-format compression library                                 |
| zvbi             | libzvbi-dev                      | Vertical Blanking Interval decoder (VBI)                      |
| snappy           | libsnappy-dev                    | Fast compression/decompression library                        |
| openjp2          | libopenjp2-7-dev                 | Development files for OpenJPEG, a JPEG 2000 image library     |
| mp3lame          | libmp3lame-dev                   | MP3 encoding library                                          |
| twolame          | libtwolame-dev                   | MPEG Audio Layer 2 encoder                                    |
| crystalhd        | libcrystalhd-dev                 | Crystal HD Video Decoder                                      |
| xvidcore         | libxvidcore-dev                  | Open source MPEG-4 video codec                                |
| shine            | libshine-dev                     | Fixed-point MP3 encoding library                              |
| ld               | libc6-dev                        | Dynamic linker/loader                                         |  
| gmp              | libgmp-dev                       | Multiprecision arithmetic library developers tools            |
| x264             | libx264-dev                      | Development files for X264 video coding library               |
| speex            | libspeex-dev                     | The Speex codec library development files                     |
| opus             | libopus-dev                      | Opus codec library development files                          |
| vpx              | libvpx-dev                       | VP8 and VP9 video codec                                       |
| v4l2             | libv4l-dev                       | Collection of video4linux support libraries                   |
| gsm              | libgsm1-dev                      | Development libraries for a GSM speech compressor             |
| wavpack          | libwavpack-dev                   | Audio codec (lossy and lossless)                              |
| webp             | libwebp-dev                      | Lossy compression of digital photographic images.             |
| webpmux          | libwebp-dev                      | Lossy compression of digital photographic images.             |
| x265             | libx265-dev                      | H.265/HEVC video stream encoder                               |
| va               | libva-dev                        | Video Acceleration (VA) API for Linux                         |
| rt               | librtaudio-dev                   | C++ library for realtime audio input/output                   |
| bz2              | libbz2-dev                       | High-quality block-sorting file compressor library            |
| openmpt          | libopenmpt-dev                   | module music library based on OpenMPT                         |
| usb              | libusb-dev                       | Userspace USB programming library development files           |
| gdcmCommon       | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmDICT         | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmDSED         | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmIOD          | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmjpeg12       | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmjpeg16       | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmjpeg8        | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmMEXD         | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| gdcmMSFF         | libgdcm2-dev                     | Grassroots DICOM development libraries and headers            |
| xml2             | libxml2-dev                      | Development files for the GNOME XML library                   |
| gdal             | libgdal-dev                      | Geospatial Data Abstraction Library                           |
| tiff             | libtiff5-dev                     | Tag Image File Format library (TIFF)                          |
| x11              | libx11-dev                       | X11 client-side library (development headers)                 |
| theora           | libtheora-dev                    | Theora Video Compression Codec                                |
| vorbis           | libvorbis-dev                    | Development files for Vorbis General Audio Compression Codec  |
| vorbisenc        | libvorbis-dev                    | Development files for Vorbis General Audio Compression Codec  |
| vorbisfile       | libvorbis-dev                    | Development files for Vorbis General Audio Compression Codec  |
| ogg              | libogg-dev                       | Ogg bitstream library development files                       |
| gstapp-1.0       | libgstreamer-plugins-base1.0-dev | GStreamer development files for libraries from the "base" set |
| gstvideo-1.0     | libgstreamer-plugins-base1.0-dev | GStreamer development files for libraries from the "base" set |
| gstreamer-1.0    | libgstreamer1.0-dev              | GStreamer core development files                              |
