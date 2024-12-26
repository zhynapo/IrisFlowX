#include "MinEnclosingCircle.hpp"
#include "base/ContoursNodeData.hpp"

const NodeDesc MinEnclosingCircle::desc =
{
    "MinEnclosingCircle",
    "MinEnclosingCircle",
    CAT_ANALY_CONTOURS,
    // inputs
    {
        PortDesc::In("contours", NodeType::Contours)
    },

    // outputs
    {
        PortDesc::Out("circle", NodeType::VisionGeometry)
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
    //----- 2. 获取轮廓输入 -----
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    if (!contourData)
    {
        setOutputData(0, nullptr);
        return;
    }
    const std::vector<std::vector<cv::Point>>& contours = contourData->value();

    if (contours.empty())
    {
        setOutputData(0, nullptr);
        return;
    }

    // Process the first contour in the list
    const std::vector<cv::Point>& contour = contours[0];

    if (contour.size() < 3) {
        auto geom = std::make_shared<VisionGeometryNodeData>();
        geom->typeValue = VisionGeometryNodeData::Type::Circle;
        geom->valid = true;
        geom->center = cv::Point2f(0, 0);
        geom->radius = 0.f;
        setOutputData(0, geom);
        return;
    }

    auto geom = std::make_shared<VisionGeometryNodeData>();

    geom->typeValue = VisionGeometryNodeData::Type::Circle;
    geom->valid = false;   // 先设为 false
    geom->contour = contour;

    cv::Point2f center;
    float radius;

    cv::minEnclosingCircle(contour, center, radius);

    geom->valid = true;
    geom->center = center;
    geom->radius = radius;

    setOutputData(0, geom);
    
}