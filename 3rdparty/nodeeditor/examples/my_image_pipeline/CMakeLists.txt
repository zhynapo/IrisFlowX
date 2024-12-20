
cmake_minimum_required(VERSION 3.21)
project(NodeEditorApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理 moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt6
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
set(OpenCV_DIR "D:/000_Workspace/300_lib/opencv/build/install")
find_package(OpenCV REQUIRED)

set(SRC
    src/main.cpp
    src/MainWindow.hpp src/MainWindow.cpp
    src/ImagePreviewDialog.hpp src/ImagePreviewDialog.cpp
    src/nodes/PixmapData.hpp src/nodes/PixmapData.cpp
    src/nodes/ImageLoaderModel.hpp src/nodes/ImageLoaderModel.cpp
    src/nodes/ImageShowModel.hpp src/nodes/ImageShowModel.cpp
    src/nodes/GaussianModel.hpp src/nodes/GaussianModel.cpp
)

qt_add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodes
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${QTNODES_TARGET}
        ${OpenCV_LIBS}
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()