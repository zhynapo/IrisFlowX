#include "DrawRotatedRect.hpp"

const NodeDesc DrawRotatedRect::desc =
{
    "DrawRotatedRect",
    "DrawRotatedRect",
    CAT_PROC_DRAW,

    // inputs
    {
        PortDesc::In("mat", NodeType::Mat),
        PortDesc::In("rotaterect", NodeType::RotatedRect)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeColor("color", "Color", QColor(255,0,0)),
        ParamDesc::makeInt("thickness", "Thickness", 2).range(1, 20),
        ParamDesc::makeInt("size", "Size", 2).range(1, 20)
    }
};

DrawRotatedRect::DrawRotatedRect()
    : BaseNodeModel(desc)
{
}

void DrawRotatedRect::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    auto rotData = std::dynamic_pointer_cast<RotatedRectNodeData>(_getInput(1));
    if (!rotData)
    {
        setOutputData(0, nullptr);
        return;
    }
    cv::RotatedRect rect = rotData->value();
 
    //----- 4. 获取参数，例如颜色 -----
    QColor color = parameterValue("color").value<QColor>();
    int thickness = parameterValue("thickness").toInt();
    int w = parameterValue("size").toInt();

    cv::Scalar cvColor(color.blue(), color.green(), color.red());   // BGR


    cv::Mat output = _input.clone();
    cv::Point2f pts[4];
    rect.points(pts);

    for (int i = 0; i < 4; i++)
        cv::line(output, pts[i], pts[(i+1)%4], cvColor, thickness);

    cv::circle(output, rect.center, 4, cvColor, thickness);

    setOutputData(0, std::make_shared<MatNodeData>(output));
}
