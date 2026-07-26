cmake_minimum_required(VERSION 3.16)

project(reboard
    VERSION 0.1.0
    DESCRIPTION "A springboard-inspired application launcher for reMarkable paper tablets"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(REBOARD_BUILD_UI "Build the Qt Quick launcher binary (requires Qt 6)" ON)
option(REBOARD_BUILD_TESTS "Build the unit tests" OFF)
option(REBOARD_COVERAGE "Instrument the build for coverage reporting" OFF)

if(REBOARD_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_subdirectory(src)

if(REBOARD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
