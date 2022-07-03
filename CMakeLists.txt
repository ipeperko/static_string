cmake_minimum_required(VERSION 3.19)
project(static_string)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    main.cpp
    )

set(HEADERS
    static_string.hpp
    static_string.ipp
    )

add_executable(static_string_test ${SOURCES} ${HEADERS})