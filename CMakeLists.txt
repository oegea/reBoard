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
if(REBOARD_BUILD_UI)
    add_subdirectory(libs/rekit)
endif()
add_subdirectory(apps/launcher)

if(REBOARD_BUILD_UI)
    add_subdirectory(apps/settings)
    add_subdirectory(apps/store)

    # Translation catalogs (ADR-0004), embedded under :/i18n in every app.
    # Done manually because qt_add_translations produced resource aliases
    # like "../../reboard_es.qm" (unreachable at :/i18n/reboard_es.qm) and
    # only attached the catalogs to the first target.
    find_package(Qt6 QUIET COMPONENTS LinguistTools)
    function(reboard_add_translations target)
        if(NOT TARGET Qt6::lrelease)
            message(WARNING "Qt LinguistTools not found: ${target} built without translations")
            return()
        endif()
        set(qm_dir ${CMAKE_BINARY_DIR}/i18n/${target})
        set(qm_file ${qm_dir}/reboard_es.qm)
        add_custom_command(
            OUTPUT ${qm_file}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${qm_dir}
            COMMAND Qt6::lrelease ${CMAKE_SOURCE_DIR}/libs/rekit/i18n/reboard_es.ts -qm ${qm_file}
            DEPENDS ${CMAKE_SOURCE_DIR}/libs/rekit/i18n/reboard_es.ts
            VERBATIM)
        qt6_add_resources(${target} "${target}_translations"
            PREFIX "/i18n" BASE ${qm_dir} FILES ${qm_file})
    endfunction()
    reboard_add_translations(reboard-ui)
    reboard_add_translations(reboard-settings)
    reboard_add_translations(reboard-store)
endif()

if(REBOARD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
