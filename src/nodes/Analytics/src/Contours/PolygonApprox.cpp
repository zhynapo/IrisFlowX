#include "PolygonApprox.hpp"
#include "base/ContoursNodeData.hpp"


const NodeDesc PolygonApprox::desc =
{
    "PolygonApprox",
    "PolygonApprox",
    CAT_ANALY_CONTOURS,

    // inputs
    {
        PortDesc::In("contours", NodeType::Contours)
    },

    // outputs
    {
        PortDesc::Out("polygons", NodeType::Contours)
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
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    if (!contourData)
    {
        setOutputData(0, nullptr);
        return;
    }

    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    std::vector<std::vector<cv::Point>> outputContours;
    
    // Process each contour in the input
    for (const auto& inputContour : inputContours) {
        std::vector<cv::Point> approximatedContour;
        
        double _epsilon = parameterValue("epsilon").value<double>();
        bool   _closed = parameterValue("closed").value<bool>();
        
        cv::approxPolyDP(inputContour, approximatedContour, _epsilon, _closed);
        outputContours.push_back(approximatedContour);
    }

    setOutputData(0, std::make_shared<ContoursNodeData>(outputContours));
}