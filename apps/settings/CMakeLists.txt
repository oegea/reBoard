# Settings app (binary: reboard-settings), story 006. A regular board
# application: launched by the daemon, closed by the home gesture.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt6_add_resources(SETTINGS_RESOURCES resources.qrc)
qt6_add_resources(SETTINGS_REKIT ${CMAKE_SOURCE_DIR}/libs/rekit/rekit.qrc)

add_executable(reboard-settings
    main.cpp
    SettingsViewModel.cpp
    ${SETTINGS_RESOURCES}
    ${SETTINGS_REKIT}
)
set_target_properties(reboard-settings PROPERTIES AUTOMOC ON)
target_compile_options(reboard-settings PRIVATE -Wall -Wextra)
target_compile_definitions(reboard-settings PRIVATE
    REBOARD_VERSION="${PROJECT_VERSION}"
    REBOARD_BUILD="${REBOARD_GIT_HASH}"
)
target_link_libraries(reboard-settings PRIVATE reboard_core Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

install(TARGETS reboard-settings RUNTIME DESTINATION bin)
