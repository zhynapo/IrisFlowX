cmake_minimum_required(VERSION 3.21)
project(IrisFlowX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 建议：将所有编译产物放在同一个输出目录，解决 Windows 下 DLL 找不到的问题
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Qt6
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.3/msvc2022_64")
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets)

# 尝试查找已安装的 QtNodes（NodeEditor）
# 若未找到，可将官方仓库作为子模块放到 3rdparty/nodeeditor 并自动 add_subdirectory
find_package(QtNodes QUIET CONFIG)

if (QtNodes_FOUND)
    message(STATUS "Found QtNodes via package config")
    # 兼容不同导出名
    if (TARGET QtNodes::QtNodes)
        set(QTNODES_TARGET QtNodes::QtNodes)
    elseif (TARGET QtNodes)
        set(QTNODES_TARGET QtNodes)
    elseif (TARGET nodes)
        set(QTNODES_TARGET nodes)
    else()
        message(FATAL_ERROR "QtNodes package found but target name is unknown")
    endif()
else()
    if (EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/nodeeditor/CMakeLists.txt)
        message(STATUS "Using QtNodes from submodule 3rdparty/nodeeditor")
        add_subdirectory(3rdparty/nodeeditor)
        if (TARGET QtNodes::QtNodes)
            set(QTNODES_TARGET QtNodes::QtNodes)
        elseif (TARGET QtNodes)
            set(QTNODES_TARGET QtNodes)
        elseif (TARGET nodes)
            set(QTNODES_TARGET nodes)
        else()
            message(FATAL_ERROR "Could not identify QtNodes target from submodule")
        endif()
    else()
        message(FATAL_ERROR "QtNodes not found. Either install it, or add as submodule at 3rdparty/nodeeditor")
    endif()
endif()

# OpenCV
set(OpenCV_DIR "D:/000_Workspace/300_lib/opencv/build_world/install")
find_package(OpenCV REQUIRED)
message(STATUS ${OpenCV_LIBS})
# IrisX
set(IrisX_DIR "C:/Program Files (x86)/IrisX")
find_package(IrisX REQUIRED)
message(STATUS ${IrisX_LIBS})


add_subdirectory(src)
