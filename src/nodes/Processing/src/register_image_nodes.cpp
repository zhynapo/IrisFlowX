#include <core/IFlowPlugin.h>
#include <core/FlowCoreExport.h>
#include "PluginExport.h"

#include "IO/ImageLoaderModel.hpp"
#include "IO/ImageShowModel.hpp"
#include "IO/FolderImageLoaderModel.hpp"    // 新增头文件引用

#include "basicops/AddWeighted.hpp"
#include "basicops/Blend.hpp"
#include "basicops/PixelWiseOp.hpp"
#include "basicops/ConvertcolorNode.hpp"
#include "basicops/LUTNodeWithWidget.hpp"
#include "basicops/Split.hpp"
#include "basicops/PyrDownNode.hpp"
#include "basicops/PyrUpNode.hpp"
#include "basicops/ExtractChannelNode.hpp"

#include "geometry/CombineAffine.hpp"
#include "geometry/ResizeNode.hpp"
#include "geometry/RotateMatrix.hpp"
#include "geometry/RotateModel.hpp"
#include "geometry/ScaleMatrix.hpp"
#include "geometry/ShearMatrix.hpp"
#include "geometry/TranslateMatrix.hpp"
#include "geometry/WarpAffine.hpp"

#include "filter/AlphaTest.hpp"
#include "filter/GaussianBlur.hpp"
#include "filter/MedianBlur.hpp"
#include "filter/BilateralFilter.hpp"
#include "filter/Test.hpp"
#include "filter/SobelGradientNode.hpp"

#include "morph/MorphologyNode.hpp"
#include "morph/ErodeNode.hpp"
#include "morph/DilateNode.hpp"

#include "draw/DrawCircle.hpp"
#include "draw/DrawRotatedRect.hpp"
#include "draw/DrawContourNode.hpp"
#include "draw/DrawGeometry.hpp"
#include "draw/DrawContours.hpp"  // 添加 DrawContours 头文件引用

extern "C" NODE_PLUGIN_API bool register_flow_nodes(Flow::FlowRegistryContext* ctx)
{
    if (ctx->abi_version != FLOW_ABI_VERSION)
        return false;

    // 自动注册：只需调用宏即可
    //IO
    REGISTER_NODE_IN_PLUGIN(ImageLoaderModel);
    REGISTER_NODE_IN_PLUGIN(ImageShowModel);
    REGISTER_NODE_IN_PLUGIN(FolderImageLoaderModel);    // 新增节点注册
    REGISTER_NODE_IN_PLUGIN(RotateModel);               // 新增RotateModel注册
    
    //basic ops
    REGISTER_NODE_IN_PLUGIN(AddWeighted);
    REGISTER_NODE_IN_PLUGIN(Blend);
    REGISTER_NODE_IN_PLUGIN(PixelWiseOp);
    REGISTER_NODE_IN_PLUGIN(ConvertcolorNode);
    REGISTER_NODE_IN_PLUGIN(LUTNodeWithWidget);
    REGISTER_NODE_IN_PLUGIN(Split);
    REGISTER_NODE_IN_PLUGIN(PyrDownNode);
    REGISTER_NODE_IN_PLUGIN(PyrUpNode);
    REGISTER_NODE_IN_PLUGIN(ExtractChannelNode);

    //geometry
    REGISTER_NODE_IN_PLUGIN(CombineAffine);
    REGISTER_NODE_IN_PLUGIN(ResizeNode);
    REGISTER_NODE_IN_PLUGIN(RotateMatrix);
    REGISTER_NODE_IN_PLUGIN(RotateModel);
    REGISTER_NODE_IN_PLUGIN(ScaleMatrix);
    REGISTER_NODE_IN_PLUGIN(ShearMatrix);
    REGISTER_NODE_IN_PLUGIN(TranslateMatrix);
    REGISTER_NODE_IN_PLUGIN(WarpAffine);
    
    //filter
    REGISTER_NODE_IN_PLUGIN(Test);
    REGISTER_NODE_IN_PLUGIN(AlphaTest);
    REGISTER_NODE_IN_PLUGIN(GaussianBlur);
    REGISTER_NODE_IN_PLUGIN(MedianBlur);
    REGISTER_NODE_IN_PLUGIN(BilateralFilter);
    REGISTER_NODE_IN_PLUGIN(SobelGradientNode);

    //morph
    REGISTER_NODE_IN_PLUGIN(MorphologyNode);
    REGISTER_NODE_IN_PLUGIN(ErodeNode);
    REGISTER_NODE_IN_PLUGIN(DilateNode);

    //draw
    REGISTER_NODE_IN_PLUGIN(DrawCircle);
    REGISTER_NODE_IN_PLUGIN(DrawRotatedRect);
    REGISTER_NODE_IN_PLUGIN(DrawContourNode);
    REGISTER_NODE_IN_PLUGIN(DrawGeometry);
    REGISTER_NODE_IN_PLUGIN(DrawContours);  // 添加 DrawContours 节点注册

    return true;
}