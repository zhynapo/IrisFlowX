#include <core/IFlowPlugin.h>
#include <core/FlowCoreExport.h>
#include "PluginExport.h"
#include "ImageLoaderModel.hpp"
#include "ImageShowModel.hpp"

#include "basicops/AddWeighted.hpp"
#include "basicops/Blend.hpp"
#include "basicops/PixelWiseOp.hpp"
#include "basicops/ConvertcolorNode.hpp"
#include "basicops/LUTNodeWithWidget.hpp"
#include "basicops/Split.hpp"

#include "geometry/ResizeNode.hpp"

#include "filter/AlphaTest.hpp"
#include "filter/GaussianBlur.hpp"
#include "filter/MedianBlur.hpp"
#include "filter/BilateralFilter.hpp"
#include "filter/Test.hpp"

#include "morph/MorphologyNode.hpp"
#include "morph/ErodeNode.hpp"
#include "morph/DilateNode.hpp"

#include "draw/DrawCircle.hpp"
#include "draw/DrawRotatedRect.hpp"
#include "draw/DrawContourNode.hpp"
extern "C" NODE_PLUGIN_API bool register_flow_nodes(Flow::FlowRegistryContext* ctx)
{
    if (ctx->abi_version != FLOW_ABI_VERSION)
        return false;

    // 自动注册：只需调用宏即可
    //IO
    REGISTER_NODE_IN_PLUGIN(ImageLoaderModel);
    REGISTER_NODE_IN_PLUGIN(ImageShowModel);
    
    //basic ops
    REGISTER_NODE_IN_PLUGIN(AddWeighted);
    REGISTER_NODE_IN_PLUGIN(Blend);
    REGISTER_NODE_IN_PLUGIN(PixelWiseOp);
    REGISTER_NODE_IN_PLUGIN(ConvertcolorNode);
    REGISTER_NODE_IN_PLUGIN(LUTNodeWithWidget);
    REGISTER_NODE_IN_PLUGIN(Split);

    //geometry
    REGISTER_NODE_IN_PLUGIN(ResizeNode);

    //filter
    REGISTER_NODE_IN_PLUGIN(Test);
    REGISTER_NODE_IN_PLUGIN(AlphaTest);
    REGISTER_NODE_IN_PLUGIN(GaussianBlur);
    REGISTER_NODE_IN_PLUGIN(MedianBlur);
    REGISTER_NODE_IN_PLUGIN(BilateralFilter);

    //morph
    REGISTER_NODE_IN_PLUGIN(MorphologyNode);
    REGISTER_NODE_IN_PLUGIN(ErodeNode);
    REGISTER_NODE_IN_PLUGIN(DilateNode);

    //draw
    REGISTER_NODE_IN_PLUGIN(DrawCircle);
    REGISTER_NODE_IN_PLUGIN(DrawRotatedRect);
    REGISTER_NODE_IN_PLUGIN(DrawContourNode);

    return true;
}