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
        PortDesc::Out("center", NodeType::Point),
        PortDesc::Out("radius", NodeType::Float)
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
    _output = _input.clone();
    cv::Point2f center;
    float radius;
    if (contour.size() < 3) {
        center = cv::Point2f(0, 0);
        radius = 0.f;
        return;
    }

    cv::minEnclosingCircle(contour, center, radius);


    setOutputData(0, std::make_shared<PointNodeData>(center));
    setOutputData(1, std::make_shared<NumberNodeData>(radius));
}
