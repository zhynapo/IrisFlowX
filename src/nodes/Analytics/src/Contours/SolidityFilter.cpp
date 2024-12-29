#include "SolidityFilter.hpp"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QJsonObject>
#include <QJsonValue>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>

const NodeDesc SolidityFilter::desc = {
    "Solidity Filter",
    "SolidityFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        ParamDesc::makeDouble("min_solidity", "Min Solidity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_solidity", "Max Solidity", 1.0).range(0.0, 1.0)
    }
};

SolidityFilter::SolidityFilter()
    : BaseNodeModel(desc)
{
}

void SolidityFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        return;
    }
    
    // Get parameters with clamping to fixed valid range [0, 1]
    double minSolidity = std::clamp(parameterValue("min_solidity").toDouble(), 0.0, 1.0);
    double maxSolidity = std::clamp(parameterValue("max_solidity").toDouble(), 0.0, 1.0);

    // Ensure min <= max
    if (minSolidity > maxSolidity) {
        std::swap(minSolidity, maxSolidity);
        setParameter("min_solidity", minSolidity);
        setParameter("max_solidity", maxSolidity);
    }

    // Filter all contours by solidity
    std::vector<std::vector<cv::Point>> filteredContours;

    for (const auto& contour : inputContours) {
        double area = cv::contourArea(contour);
        if (area <= 0) continue;

        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        double hullArea = cv::contourArea(hull);
        if (hullArea <= 0) continue;

        double solidity = area / hullArea;
        if (solidity >= minSolidity && solidity <= maxSolidity) {
            filteredContours.push_back(contour);
        }
    }

    // Output null if no contours match
    if (filteredContours.empty()) {
        setOutputData(0, nullptr);
        return;
    }

    // Create output with filtered contours
    auto outputData = std::make_shared<ContoursNodeData>(filteredContours);
    setOutputData(0, outputData);
}