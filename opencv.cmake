# CMake script to handle libopencv dependencies via Debian/Raspbian/Ubuntu installed packages.

message(STATUS "IMPORTANT: Be sure to compile this project using the Clang C Language Family Frontend for LLVM.")
message(STATUS "gcc/g++ is known to fail on Ubuntu 18 LTS. clang version 6.0.0-1ubuntu2 is known to work.")
message(STATUS "https://clang.llvm.org/get_started.html")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -v -ltbb -lz -lm -lswresample \
                           -lopencv_core -lopencv_videoio -lopencv_highgui -ldc1394  \
                           -lopencv_imgcodecs -lopencv_imgproc -ljpeg -lpng -ltiff \
                           -lavformat -lavcodec -lswscale -lavutil -lavdevice -lavfilter \
                           -lssh -llzma -lgsm -lzvbi -lsnappy -lopenjp2 -lv4l2 \
                           -lshine -lxvidcore -lwavpack -lwebp -lwebpmux -lx265 -lgmp \
                           -lva -lrt -lX11 -lbz2 -lopenmpt -lusb -lxml2 -lgdcmCommon \
                           -lgdcmDICT -lgdcmDSED -lgdcmIOD -lgdcmjpeg12 -lgdcmjpeg16 \
                           -lgdcmjpeg8 -lgdcmMEXD -lgdcmMSFF -lgdal -lpostproc \
                           -lmp3lame -ltwolame -lvorbis -lvorbisenc -lvorbisfile -lx264 \
                           -lx265 -lspeex -lopus -lvpx -ltheora -logg -lgstvideo-1.0 \
                           -lgstvideo-1.0 -lgstreamer-1.0")

set(OPENCV_INCLUDE_DIR /usr/include/opencv4)