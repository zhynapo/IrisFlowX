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
        PortDesc::Out("moments", NodeType::VisionGeometry)
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
    auto geom = std::make_shared<VisionGeometryNodeData>();

    geom->typeValue = VisionGeometryNodeData::Type::RotatedRect;
    geom->valid = false;   // 先设为 false
    cv::RotatedRect rr = cv::minAreaRect(contour);
    geom->center = rr.center;
    geom->size = rr.size;
    geom->angle = rr.angle;
    geom->valid = true;    // 计算成功，设为 true

    setOutputData(0, geom);
}
