# Shared business core (ADR-0002): domain + application + infrastructure.
# Qt-free, so it builds and unit-tests on any Linux host.
add_library(reboard_core STATIC
    domain/entities/Board.cpp
    domain/services/GestureDetector.cpp
    infrastructure/input/EvdevKeyboardDetector.cpp
    infrastructure/input/EvdevPowerButton.cpp
    infrastructure/input/EvdevTouchScreen.cpp
    infrastructure/manifests/ManifestParser.cpp
    infrastructure/packages/FilePackageRepository.cpp
    infrastructure/packages/PackageTemplating.cpp
    infrastructure/processes/PosixProcessRepository.cpp
    infrastructure/processes/UiDirective.cpp
    infrastructure/processes/UiProcessController.cpp
    infrastructure/repositories/FileApplicationRepository.cpp
    infrastructure/system/DeviceSlug.cpp
)
target_include_directories(reboard_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(reboard_core PRIVATE -Wall -Wextra)
