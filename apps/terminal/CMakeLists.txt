# Terminal app (binary: reboard-terminal), story 013. Distributed through
# the App Store, not with the base system image.

# The emulator is Qt-free and always built so it can be unit-tested on any
# host.
add_library(terminal_emulator STATIC
    emulator/TerminalEmulator.cpp
)
target_include_directories(terminal_emulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(terminal_emulator PRIVATE -Wall -Wextra)

if(REBOARD_BUILD_UI)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

    qt6_add_resources(TERMINAL_RESOURCES resources.qrc)
    qt6_add_resources(TERMINAL_REKIT ${CMAKE_SOURCE_DIR}/libs/rekit/rekit.qrc)

    add_executable(reboard-terminal
        main.cpp
        PtySession.cpp
        TerminalViewModel.cpp
        ${TERMINAL_RESOURCES}
        ${TERMINAL_REKIT}
    )
    set_target_properties(reboard-terminal PROPERTIES AUTOMOC ON)
    target_compile_options(reboard-terminal PRIVATE -Wall -Wextra)
    target_compile_definitions(reboard-terminal PRIVATE REBOARD_VERSION="${PROJECT_VERSION}")
    # `util` provides forkpty.
    target_link_libraries(reboard-terminal PRIVATE terminal_emulator rekit_qt util
        Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

    install(TARGETS reboard-terminal RUNTIME DESTINATION bin)
endif()
