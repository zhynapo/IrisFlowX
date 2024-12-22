#include "MomentsNode.hpp"

const NodeDesc MomentsNode::desc =
{
    "MomentsNode",
    "Moments Calculator",
    "Analytics/Features",

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("moments", NodeType::VisionGeometry)
    },

    // parameters
    {
        ParamDesc::makeBool("binaryImage", "binaryImage", false),
        ParamDesc::makeCombo("method", "method", QVector<QString>{ QString("All"), QString("SpatialOnly"), QString("CentralOnly"), QString("NormalizedCentralOnly") }, QVector<int>{ 0, 1, 2, 3 }, 0)
    }
};

MomentsNode::MomentsNode()
    : BaseNodeModel(desc)
{
}

void MomentsNode::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    cv::Mat gray;
    if(_input.channels() > 1)
        cv::cvtColor(_input, gray, cv::COLOR_BGR2GRAY);
    else
        gray = _input;

    cv::Moments m = cv::moments(gray, parameterValue("binaryImage").toBool());
    auto geom = std::make_shared<VisionGeometryNodeData>();

    geom->typeValue = VisionGeometryNodeData::Type::Circle;
    geom->valid = false;   // 先设为 false

    if (m.m00 > 1e-5)
    {
        geom->valid = true;

        // centroid
        geom->center = cv::Point2f(
            float(m.m10 / m.m00),
            float(m.m01 / m.m00)
        );

        // orientation angle
        geom->angle = 0.5f * std::atan2(
            2.0 * m.mu11,
            m.mu20 - m.mu02
        );

        // approximate size as radius
        double area = m.m00;
        geom->radius = float(std::sqrt(area / CV_PI));
    }

    setOutputData(0, geom);
}
