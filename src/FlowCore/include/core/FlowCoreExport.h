#pragma once

#include <QtCore/qglobal.h>

/**
 * FLOWCORE_LIBRARY 这个宏由 CMake 在编译 FlowCore 目标时定义。
 * * - 当编译 FlowCore 库本身时：符号被标记为导出 (dllexport)。
 * - 当 Nodes 插件或 App 引用此头文件时：符号被标记为导入 (dllimport)。
 */
#if defined(FLOWCORE_LIBRARY)
#  define FLOWCORE_API Q_DECL_EXPORT
#else
#  define FLOWCORE_API Q_DECL_IMPORT
#endif