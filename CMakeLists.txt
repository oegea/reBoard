cmake_minimum_required(VERSION 3.16)

project(reboard
    VERSION 0.1.0
    DESCRIPTION "The unified visual layer for reMarkable paper tablets"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(REBOARD_BUILD_UI "Build the Qt Quick binaries (requires Qt 6)" ON)
option(REBOARD_BUILD_TESTS "Build the unit tests" OFF)
option(REBOARD_COVERAGE "Instrument the build for coverage reporting" OFF)

if(REBOARD_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Short git hash of the running build, shown in the Settings app.
execute_process(
    COMMAND git rev-parse --short=8 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE REBOARD_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
if(NOT REBOARD_GIT_HASH)
    set(REBOARD_GIT_HASH "unknown")
endif()

add_subdirectory(libs/core)
add_subdirectory(apps/launcher)

if(REBOARD_BUILD_UI)
    add_subdirectory(apps/settings)

    # Translation catalogs (ADR-0004), embedded under :/i18n in every app.
    find_package(Qt6 QUIET COMPONENTS LinguistTools)
    if(TARGET Qt6::lrelease)
        qt6_add_translations(reboard-ui reboard-settings
            TS_FILES libs/rekit/i18n/reboard_es.ts)
    else()
        message(WARNING "Qt LinguistTools not found: building without translations")
    endif()
endif()

if(REBOARD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
