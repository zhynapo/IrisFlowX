cmake_minimum_required(VERSION 3.16)

project(FlowCore VERSION 1.0 LANGUAGES CXX)

# 启用 moc / rcc / uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---------------------------------------------------
# 收集 cpp
# ---------------------------------------------------
file(GLOB_RECURSE FLOWCORE_CPP
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ---------------------------------------------------
# 收集头文件（只是为了 install 和 IDE 展示，不用于编译）
# ---------------------------------------------------
file(GLOB_RECURSE FLOWCORE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# ---------------------------------------------------
# FlowCore 库
# ---------------------------------------------------
add_library(FlowCore SHARED
    ${FLOWCORE_CPP}
    ${FLOWCORE_HEADERS}   # 不会影响 AUTOMOC，因为我们要显式 include 目录
)

# ---------------------------------------------------
# 导出宏
# ---------------------------------------------------
target_compile_definitions(FlowCore PRIVATE FLOWCORE_LIBRARY)

# ---------------------------------------------------
# Qt
# ---------------------------------------------------
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

target_link_libraries(FlowCore
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        QtNodes::QtNodes
        ${OpenCV_LIBS}
)

# ---------------------------------------------------
# 头文件路径
# （最关键——这样插件就可以这样 include： #include <Flow/model/BaseNodeModel.hpp>）
# ---------------------------------------------------
target_include_directories(FlowCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
