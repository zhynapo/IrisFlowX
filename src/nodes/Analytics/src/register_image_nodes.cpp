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
#include "Contours/CombinedContoursNode.hpp"
#include "Contours/AreaPerimeterFilter.hpp"
#include "Contours/ShapeMetricsFilter.hpp"
#include "Contours/EccentricityFilter.hpp"

#include "Features/MomentsNode.hpp"
#include "Features/HarrisCorner.hpp"
#include "Features/FastCorner.hpp"
#include "Features/GfttCorner.hpp"
#include "Features/OrbFeature.hpp"
#include "Features/SiftFeature.hpp"
#include "Features/SurfFeature.hpp"

#include "segment/ColorAnomalyDetectionNode.hpp"

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
    REGISTER_NODE_IN_PLUGIN(CombinedContoursNode);

    REGISTER_NODE_IN_PLUGIN(AreaPerimeterFilter);
    REGISTER_NODE_IN_PLUGIN(ShapeMetricsFilter);
    REGISTER_NODE_IN_PLUGIN(EccentricityFilter);

    //features
    REGISTER_NODE_IN_PLUGIN(MomentsNode);
    REGISTER_NODE_IN_PLUGIN(HarrisCorner);
    REGISTER_NODE_IN_PLUGIN(FastCorner);
    REGISTER_NODE_IN_PLUGIN(GfttCorner);
    REGISTER_NODE_IN_PLUGIN(OrbFeature);
    REGISTER_NODE_IN_PLUGIN(SiftFeature);
    REGISTER_NODE_IN_PLUGIN(SurfFeature);

    //segmentation
    REGISTER_NODE_IN_PLUGIN(ColorAnomalyDetectionNode);


    return true;
}