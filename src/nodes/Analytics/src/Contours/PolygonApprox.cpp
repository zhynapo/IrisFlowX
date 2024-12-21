#include "PolygonApprox.hpp"


const NodeDesc PolygonApprox::desc =
{
    "PolygonApprox",
    "PolygonApprox",
    CAT_ANALY_CONTOURS,

    // inputs
    {
        PortDesc::In("points", NodeType::Points)
    },

    // outputs
    {
        PortDesc::Out("out", NodeType::Points)
    },

    // parameters
    {
        ParamDesc::makeDouble("epsilon", "epsilon", 1.0),
        ParamDesc::makeBool("closed", "closed", true)
    }
};

PolygonApprox::PolygonApprox()
    : BaseNodeModel(desc)
{
}

void PolygonApprox::process()
{
    auto matData = std::dynamic_pointer_cast<PointsNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    std::vector<cv::Point> _input = matData->value();
    std::vector<cv::Point> _output;
    // TODO: implement algorithm
    double _epsilon = parameterValue("epsilon").value<double>();
    bool   _closed = parameterValue("closed").value<bool>();
    cv::approxPolyDP(_input, _output,
        _epsilon, _closed);

    setOutputData(0, std::make_shared<PointsNodeData>(_output));
}
