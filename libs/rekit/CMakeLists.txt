# Qt-side helpers of the reKit UI kit shared by every app binary
# (the QML components themselves ship via rekit.qrc).
find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(rekit_qt STATIC
    qt/LocaleResolver.cpp
)
target_include_directories(rekit_qt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/qt)
target_compile_options(rekit_qt PRIVATE -Wall -Wextra)
target_link_libraries(rekit_qt PUBLIC Qt6::Core)
