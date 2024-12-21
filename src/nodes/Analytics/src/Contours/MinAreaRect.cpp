#include "MinAreaRect.hpp"

const NodeDesc MinAreaRect::desc =
{
    "MinAreaRect",
    "MinAreaRect",
    CAT_ANALY_CONTOURS,

    // inputs
    {
        PortDesc::In("points", NodeType::Points)
    },

    // outputs
    {
        PortDesc::Out("rotaterect", NodeType::RotatedRect)
    },

    // parameters
    {
        ParamDesc::makeDouble("epsilon", "epsilon", 1.0),
        ParamDesc::makeBool("closed", "closed", true)
    }
};

MinAreaRect::MinAreaRect()
    : BaseNodeModel(desc)
{
}

void MinAreaRect::process()
{
    auto ptsData = std::dynamic_pointer_cast<PointsNodeData>(_getInput(0));
    if (!ptsData)
    {
        setOutputData(0, nullptr);
        return;
    }
    const std::vector<cv::Point>& contour = ptsData->value();

    if (contour.empty())
    {
        setOutputData(0, nullptr);
        return;
    }
    
    cv::RotatedRect rr = cv::minAreaRect(contour);

    setOutputData(0, std::make_shared<RotatedRectNodeData>(rr));
}
