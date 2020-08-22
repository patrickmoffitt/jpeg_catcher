# CMake script to handle jsoncpp dependency via Debian/Raspbian/Ubuntu installed package.

find_library(JSONCPP_LIBRARY libjsoncpp.a)
if (JSONCPP_LIBRARY)
    message(STATUS "Found STATIC libjsoncpp library.")
    target_include_directories(${PROJECT_NAME} PUBLIC /usr/include/jsoncpp )
    target_link_libraries(${PROJECT_NAME} ${JSONCPP_LIBRARY})
else()
    message(STATUS "CMake could not find library libjsoncpp. Please install it manually and re-run CMake.")
    message(STATUS "sudo apt-get install libjsoncpp1 libjsoncpp-dev")
endif()

