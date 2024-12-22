#include "MinEnclosingCircle.hpp"

const NodeDesc MinEnclosingCircle::desc =
{
    "MinEnclosingCircle",
    "MinEnclosingCircle",
    CAT_ANALY_CONTOURS,
    // inputs
    {
        PortDesc::In("points", NodeType::Points)
    },

    // outputs
    {
        PortDesc::Out("moments", NodeType::VisionGeometry)
        //PortDesc::Out("center", NodeType::Point),
        //PortDesc::Out("radius", NodeType::Float)
    },

    // parameters
    {
        ParamDesc::makeDouble("epsilon", "epsilon", 1.0),
        ParamDesc::makeBool("closed", "closed", true)
    }
};

MinEnclosingCircle::MinEnclosingCircle()
    : BaseNodeModel(desc)
{
}

void MinEnclosingCircle::process()
{
    //----- 2. 获取点集输入 -----
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

    // TODO: implement algorithm
    auto geom = std::make_shared<VisionGeometryNodeData>();

    geom->typeValue = VisionGeometryNodeData::Type::Circle;
    geom->valid = false;   // 先设为 false
    geom->contour = contour;

    cv::Point2f center;
    float radius;
    if (contour.size() < 3) {
        geom->valid = true;
        geom->center = cv::Point2f(0, 0);
        geom->radius = 0.f;
        return;
    }

    cv::minEnclosingCircle(contour, center, radius);

    geom->valid = true;
    geom->center = center;
    geom->radius = radius;

    setOutputData(0, geom);
    
}
