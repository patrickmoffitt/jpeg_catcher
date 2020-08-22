# CMake script to handle libmagic dependency via Debian/Raspbian/Ubuntu installed package.

find_library(LIBMAGIC_LIBRARY libmagic.a)
if (LIBMAGIC_LIBRARY)
    message(STATUS "Found STATIC libmagic library.")
    target_include_directories(${PROJECT_NAME} PUBLIC /usr/include)  # magic.h
    target_link_libraries(${PROJECT_NAME} ${LIBMAGIC_LIBRARY})
else()
    message(STATUS "CMake could not find library libmagic-dev. Please install it manually and re-run CMake.")
    message(STATUS "sudo apt-get install libmagic-dev")
endif()

find_library(ZLIB libz.so)
if (ZLIB)
    message(STATUS "Found SHARED libz library needed by magic.")
    target_link_libraries(${PROJECT_NAME} ${ZLIB})
else()
    message(STATUS "CMake could not find library libz. Please install it manually and re-run CMake.")
    message(STATUS "sudo apt-get install zlib1g-dev")
endif()