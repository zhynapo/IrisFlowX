#include "DrawCircle.hpp"

const NodeDesc DrawCircle::desc =
{
    "DrawCircle",
    "DrawCircle",
    CAT_PROC_DRAW,

    // inputs
    {
        PortDesc::In("mat", NodeType::Mat),
        PortDesc::In("center", NodeType::Point),
        PortDesc::In("radius", NodeType::Float)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeColor("color", "Color", QColor(255,0,0)),
        ParamDesc::makeInt("thickness", "Thickness", 2).setRange(1, 20),
        ParamDesc::makeInt("size", "Size", 2).setRange(1, 20)
    }
};

DrawCircle::DrawCircle()
    : BaseNodeModel(desc)
{
}

void DrawCircle::process()
{
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat inputMat = matData->mat();

    auto ptsData = std::dynamic_pointer_cast<PointNodeData>(_getInput(1));
    if (!ptsData)
    {
        setOutputData(0, nullptr);
        return;
    }
    cv::Point2f ceter = ptsData->value();

    auto radiusData = std::dynamic_pointer_cast<NumberNodeData>(_getInput(2));
    if (!radiusData)
    {
        setOutputData(0, nullptr);
        return;
    }
    float radius = radiusData->value();
 
    //----- 4. 获取参数，例如颜色 -----
    QColor color = parameterValue("color").value<QColor>();
    int thickness = parameterValue("thickness").toInt();
    int w = parameterValue("size").toInt();

    cv::Scalar cvColor(color.blue(), color.green(), color.red());   // BGR

    // TODO: implement algorithm
    //----- 3. 克隆输入图像 -----
    cv::Mat output = inputMat.clone();


    //----- 4. 获取参数，例如颜色 -----
    circle(output, cv::Point(cvRound(ceter.x), cvRound(ceter.y)), radius, cvColor, thickness);
    cv::Point pt1, pt2;
    pt1.x = ceter.x - w; pt1.y = ceter.y;
    pt2.x = ceter.x + w; pt2.y = ceter.y;
    line(output, pt1, pt2, cvColor, thickness);
    pt1.x = ceter.x; pt1.y = ceter.y - w;
    pt2.x = ceter.x; pt2.y = ceter.y + w;
    line(output, pt1, pt2, cvColor, thickness);
    setOutputData(0, std::make_shared<MatNodeData>(output));
}
