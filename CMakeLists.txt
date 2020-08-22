cmake_minimum_required(VERSION 3.13.4)
project(jpeg_catcher)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)

#set(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(opencv.cmake)

find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        http_parser.hpp http_parser.cpp
        hex_dump.hpp hex_dump.cpp
        http_io.cpp http_io.hpp
        string_util.cpp string_util.hpp
        magic_mime.hpp magic_mime.cpp
        base64.hpp base64.cpp
        opencv_util.hpp opencv_util.cpp
        jsonxx.h jsonxx.cc
        debug_output.hpp mjpeg_utils.cpp mjpeg_utils.hpp)

# include(jsoncpp.cmake)

include(libmagic.cmake)

target_include_directories(${PROJECT_NAME} BEFORE PUBLIC ${CURL_INCLUDE_DIR}/curl)
target_link_libraries(${PROJECT_NAME} curl)

get_property(dirs TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIRECTORIES)
message(STATUS "INCLUDE_DIRECTORIES: ${dirs}")

get_property(libs TARGET ${PROJECT_NAME} PROPERTY LINK_LIBRARIES)
message(STATUS "LINK_LIBRARIES: ${libs}")