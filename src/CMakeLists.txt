# Business core: domain + application + infrastructure. No Qt dependency, so
# it can be built and unit-tested on any Linux host.
add_library(reboard_core STATIC
    domain/entities/Board.cpp
    domain/services/GestureDetector.cpp
    infrastructure/input/EvdevTouchScreen.cpp
    infrastructure/manifests/ManifestParser.cpp
    infrastructure/processes/PosixProcessRepository.cpp
    infrastructure/repositories/FileApplicationRepository.cpp
)
target_include_directories(reboard_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(reboard_core PRIVATE -Wall -Wextra)

# Launcher binary: Qt Quick UI rendered through the official reMarkable
# `epaper` platform plugin.
if(REBOARD_BUILD_UI)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

    qt6_add_resources(REBOARD_RESOURCES ui/resources.qrc)

    add_executable(reboard
        ui/main.cpp
        ui/BoardViewModel.cpp
        ui/LauncherController.cpp
        ui/TouchGestureMonitor.cpp
        ${REBOARD_RESOURCES}
    )
    set_target_properties(reboard PROPERTIES AUTOMOC ON)
    target_compile_options(reboard PRIVATE -Wall -Wextra)
    target_compile_definitions(reboard PRIVATE REBOARD_VERSION="${PROJECT_VERSION}")
    target_link_libraries(reboard PRIVATE reboard_core Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

    install(TARGETS reboard RUNTIME DESTINATION bin)
endif()
