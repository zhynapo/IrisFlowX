#include <core/IFlowPlugin.h>
#include <core/FlowCoreExport.h>
#include "PluginExport.h"
#include "ImageLoaderModel.hpp"
#include "ImageShowModel.hpp"
#include "Gaussianblur.hpp"
#include "Blend.hpp"

extern "C" NODE_PLUGIN_API bool register_flow_nodes(Flow::FlowRegistryContext* ctx)
{
    if (ctx->abi_version != FLOW_ABI_VERSION)
        return false;

    // 自动注册：只需调用宏即可
    REGISTER_NODE_IN_PLUGIN(ImageLoaderModel);
    REGISTER_NODE_IN_PLUGIN(ImageShowModel);
    REGISTER_NODE_IN_PLUGIN(Gaussianblur);
    REGISTER_NODE_IN_PLUGIN(Blend);

    return true;
}