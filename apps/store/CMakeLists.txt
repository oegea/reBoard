# App Store (binary: reboard-store), story 009. A regular board application
# launched by the daemon; installs/uninstalls store packages through the
# core use cases.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

qt6_add_resources(STORE_RESOURCES resources.qrc)
qt6_add_resources(STORE_REKIT ${CMAKE_SOURCE_DIR}/libs/rekit/rekit.qrc)

add_executable(reboard-store
    main.cpp
    StoreViewModel.cpp
    ${STORE_RESOURCES}
    ${STORE_REKIT}
)
set_target_properties(reboard-store PROPERTIES AUTOMOC ON)
target_compile_options(reboard-store PRIVATE -Wall -Wextra)
target_compile_definitions(reboard-store PRIVATE REBOARD_VERSION="${PROJECT_VERSION}")
target_link_libraries(reboard-store PRIVATE reboard_core
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Network)

install(TARGETS reboard-store RUNTIME DESTINATION bin)
