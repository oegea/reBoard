find_package(Threads REQUIRED)

# Resident daemon (binary: reboard). Qt-free on purpose — it must keep
# running while other applications own the e-paper display.
add_executable(reboard daemon/main.cpp)
target_compile_options(reboard PRIVATE -Wall -Wextra)
target_link_libraries(reboard PRIVATE reboard_core Threads::Threads)
install(TARGETS reboard RUNTIME DESTINATION bin)

# Short-lived launcher UI (binary: reboard-ui): draws the board, prints a
# launch directive to stdout and exits so the display lock is released.
if(REBOARD_BUILD_UI)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

    qt6_add_resources(LAUNCHER_RESOURCES ui/resources.qrc)
    qt6_add_resources(LAUNCHER_REKIT ${CMAKE_SOURCE_DIR}/libs/rekit/rekit.qrc)

    add_executable(reboard-ui
        ui/main.cpp
        ui/BoardViewModel.cpp
        ui/LauncherController.cpp
        ui/PowerButtonMonitor.cpp
        ${LAUNCHER_RESOURCES}
        ${LAUNCHER_REKIT}
    )
    set_target_properties(reboard-ui PROPERTIES AUTOMOC ON)
    target_compile_options(reboard-ui PRIVATE -Wall -Wextra)
    target_compile_definitions(reboard-ui PRIVATE REBOARD_VERSION="${PROJECT_VERSION}")
    target_link_libraries(reboard-ui PRIVATE reboard_core rekit_qt Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

    install(TARGETS reboard-ui RUNTIME DESTINATION bin)
endif()
