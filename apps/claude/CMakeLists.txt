# Claude Code companion (binary: reboard-claude), story 016. Talks the
# official stream-json interface over SSH; distributed through the store.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt6_add_resources(CLAUDE_RESOURCES resources.qrc)
qt6_add_resources(CLAUDE_REKIT ${CMAKE_SOURCE_DIR}/libs/rekit/rekit.qrc)

add_executable(reboard-claude
    main.cpp
    ClaudeSession.cpp
    ClaudeViewModel.cpp
    ${CLAUDE_RESOURCES}
    ${CLAUDE_REKIT}
)
set_target_properties(reboard-claude PROPERTIES AUTOMOC ON)
target_compile_options(reboard-claude PRIVATE -Wall -Wextra)
target_compile_definitions(reboard-claude PRIVATE REBOARD_VERSION="${PROJECT_VERSION}")
target_link_libraries(reboard-claude PRIVATE rekit_qt Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

install(TARGETS reboard-claude RUNTIME DESTINATION bin)
