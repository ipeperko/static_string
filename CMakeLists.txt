cmake_minimum_required(VERSION 3.16)
project(static_string LANGUAGES CXX)

option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(HEADERS
    include/static_string/static_string.hpp
    )

add_library(${CMAKE_PROJECT_NAME}
        INTERFACE
        )

target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE include/)

# --------------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------------

add_library(static_string::static_string ALIAS static_string)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/static_string
        DESTINATION include
)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


