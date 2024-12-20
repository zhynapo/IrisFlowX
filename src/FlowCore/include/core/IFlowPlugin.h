#pragma once
#include <QString>
#include <functional>
#include <memory>

#include "FlowCoreExport.h"
#include "model/BaseNodeModel.hpp"

namespace Flow {

// 模块节点信息（类似 Atlas ModuleInfo）
struct NodeModuleInfo {
    QString name;     // 节点名称
    uint32_t version; // 节点版本
    std::function<std::unique_ptr<BaseNodeModel>()> factory; // 构造节点实例的工厂函数
    QString category; // 节点分类（支持多级，如 "Image/IO"）
};

// DLL 与主框架协作的数据结构
struct FlowRegistryContext {
    int abi_version;

    // DLL 将内部节点通过此函数“报备”给主程序
    std::function<void(const NodeModuleInfo&)> register_func;
};

} // namespace Flow

// ABI 版本
#define FLOW_ABI_VERSION 1

// DLL 必须导出此函数
typedef bool (*RegisterFlowModuleFn)(Flow::FlowRegistryContext*);

//
// 便利注册宏（节点插件专用）
//
// 用法：
//     REGISTER_NODE_IN_PLUGIN(ImageLoaderModel);
//
#define REGISTER_NODE_IN_PLUGIN(T)                                       \
    do {                                                                  \
        Flow::NodeModuleInfo info;                                        \
        info.name = T::desc.modelName;                                         \
        info.version = T::desc.version;                                   \
        info.category = T::desc.category;                                 \
        info.factory = []() { return std::make_unique<T>(); };            \
        ctx->register_func(info);                                         \
    } while (0)