include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(reboard_tests
    domain/ValueObjectsTest.cpp
    domain/BoardTest.cpp
    domain/GestureDetectorTest.cpp
    application/UseCasesTest.cpp
    infrastructure/ManifestParserTest.cpp
    infrastructure/PosixProcessRepositoryTest.cpp
    infrastructure/RepositoriesTest.cpp
)
target_link_libraries(reboard_tests PRIVATE reboard_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(reboard_tests)
