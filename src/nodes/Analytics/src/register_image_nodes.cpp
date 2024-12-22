#include <core/IFlowPlugin.h>
#include <core/FlowCoreExport.h>
#include "PluginExport.h"

#include "Threshold/ThresholdModel.hpp"

#include "Contours/FindContoursNode.hpp"
#include "Contours/ContoursSelectNode.hpp"
#include "Contours/ContourAreaNode.hpp"
#include "Contours/MinEnclosingCircle.hpp"
#include "Contours/MinAreaRect.hpp"
#include "Contours/PolygonApprox.hpp"
#include "features/MomentsNode.hpp"


extern "C" NODE_PLUGIN_API bool register_flow_nodes(Flow::FlowRegistryContext* ctx)
{
    if (ctx->abi_version != FLOW_ABI_VERSION)
        return false;

    // 自动注册：只需调用宏即可
    //Threshold
    REGISTER_NODE_IN_PLUGIN(ThresholdModel);

    //Contours
    REGISTER_NODE_IN_PLUGIN(FindContoursNode);
    REGISTER_NODE_IN_PLUGIN(ContoursSelectNode);
    REGISTER_NODE_IN_PLUGIN(ContourAreaNode);
    REGISTER_NODE_IN_PLUGIN(MinEnclosingCircle);
    REGISTER_NODE_IN_PLUGIN(MinAreaRect);
    REGISTER_NODE_IN_PLUGIN(PolygonApprox);

    //features
    REGISTER_NODE_IN_PLUGIN(MomentsNode);

    return true;
}