#include "ShapeMetricsFilter.hpp"
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

const NodeDesc ShapeMetricsFilter::desc = {
    "Shape Metrics Filter",
    "ShapeMetricsFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        // Boolean parameters to enable/disable filters
        ParamDesc::makeBool("enable_circularity", "Enable Circularity Filter", false),
        ParamDesc::makeDouble("min_circularity", "Min Circularity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_circularity", "Max Circularity", 1.0).range(0.0, 1.0),
        
        ParamDesc::makeBool("enable_solidity", "Enable Solidity Filter", false),
        ParamDesc::makeDouble("min_solidity", "Min Solidity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_solidity", "Max Solidity", 1.0).range(0.0, 1.0),
        
        ParamDesc::makeBool("enable_extent", "Enable Extent Filter", false),
        ParamDesc::makeDouble("min_extent", "Min Extent", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_extent", "Max Extent", 1.0).range(0.0, 1.0),
        
        // Index parameter for selecting from filtered results
        ParamDesc::makeInt("index", "Contour Index", 0).range(0, 100),
        
        // Parameters to display contour metrics using labels
        ParamDesc::makeLabel("circularity_label", "Circularity", "0.0"),
        ParamDesc::makeLabel("solidity_label", "Solidity", "0.0"),
        ParamDesc::makeLabel("extent_label", "Extent", "0.0")
    }
};

ShapeMetricsFilter::ShapeMetricsFilter()
    : BaseNodeModel(desc)
{
}

void ShapeMetricsFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        setParameter("circularity_label", "Circularity: 0.000");
        setParameter("solidity_label", "Solidity: 0.000");
        setParameter("extent_label", "Extent: 0.000");
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        setParameter("circularity_label", "Circularity: 0.000");
        setParameter("solidity_label", "Solidity: 0.000");
        setParameter("extent_label", "Extent: 0.000");
        return;
    }
    
    // Get parameters
    bool enableCircularity = parameterValue("enable_circularity").toBool();
    double minCircularity = parameterValue("min_circularity").toDouble();
    double maxCircularity = parameterValue("max_circularity").toDouble();
    
    bool enableSolidity = parameterValue("enable_solidity").toBool();
    double minSolidity = parameterValue("min_solidity").toDouble();
    double maxSolidity = parameterValue("max_solidity").toDouble();
    
    bool enableExtent = parameterValue("enable_extent").toBool();
    double minExtent = parameterValue("min_extent").toDouble();
    double maxExtent = parameterValue("max_extent").toDouble();
    
    int index = parameterValue("index").toInt();
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply filters to each contour
    for (const auto& contour : inputContours) {
        bool passesAllFilters = true;
        
        // Circularity filter
        if (passesAllFilters && enableCircularity) {
            double area = cv::contourArea(contour);
            double perimeter = cv::arcLength(contour, true);
            if (perimeter > 0) {
                double circularity = 4 * CV_PI * area / (perimeter * perimeter);
                if (circularity < minCircularity || circularity > maxCircularity) {
                    passesAllFilters = false;
                }
            } else {
                passesAllFilters = false; // Invalid contour
            }
        }
        
        // Solidity filter
        if (passesAllFilters && enableSolidity) {
            std::vector<cv::Point> hull;
            cv::convexHull(contour, hull);
            double hullArea = cv::contourArea(hull);
            if (hullArea > 0) {
                double solidity = cv::contourArea(contour) / hullArea;
                if (solidity < minSolidity || solidity > maxSolidity) {
                    passesAllFilters = false;
                }
            } else {
                passesAllFilters = false; // Invalid hull
            }
        }
        
        // Extent filter
        if (passesAllFilters && enableExtent) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            double boundingArea = boundingRect.width * boundingRect.height;
            if (boundingArea > 0) {
                double extent = cv::contourArea(contour) / boundingArea;
                if (extent < minExtent || extent > maxExtent) {
                    passesAllFilters = false;
                }
            } else {
                passesAllFilters = false; // Invalid bounding rect
            }
        }
        
        if (passesAllFilters) {
            filteredContours.push_back(contour);
        }
    }
    
    // Update parameter ranges based on filtered contour count
    int n = filteredContours.size();
    setParamRange("index", 0, n - 1);
    setParamEnabled("index", n > 0);
    
    std::vector<std::vector<cv::Point>> outputContours;
    
    if (filteredContours.empty()) {
        // No contours passed all filters, return null
        setOutputData(0, nullptr);
        setParameter("circularity_label", "Circularity: 0.000");
        setParameter("solidity_label", "Solidity: 0.000");
        setParameter("extent_label", "Extent: 0.000");
        return;
    }
    
    std::vector<cv::Point> selectedContour;
    if (index >= 0 && index < n) {
        selectedContour = filteredContours[index];
        outputContours.push_back(selectedContour);
    } else {
        // If index is out of range, default to the first contour
        selectedContour = filteredContours[0];
        outputContours.push_back(selectedContour);
    }
    
    // Calculate metrics for the selected contour
    double area = cv::contourArea(selectedContour);
    double perimeter = cv::arcLength(selectedContour, true);
    
    double circularity = 0.0;
    if (perimeter > 0) {
        circularity = 4 * CV_PI * area / (perimeter * perimeter);
    }
    
    double solidity = 0.0;
    std::vector<cv::Point> hull;
    cv::convexHull(selectedContour, hull);
    double hullArea = cv::contourArea(hull);
    if (hullArea > 0) {
        solidity = area / hullArea;
    }
    
    cv::Rect boundingRect = cv::boundingRect(selectedContour);
    double boundingArea = boundingRect.width * boundingRect.height;
    double extent = 0.0;
    if (boundingArea > 0) {
        extent = area / boundingArea;
    }
    
    setParameter("circularity_label", QString("Circularity: %1").arg(circularity, 0, 'f', 3));
    setParameter("solidity_label", QString("Solidity: %1").arg(solidity, 0, 'f', 3));
    setParameter("extent_label", QString("Extent: %1").arg(extent, 0, 'f', 3));
    
    // Create contour data
    auto outputData = std::make_shared<ContoursNodeData>(outputContours);
    
    // Output the results
    setOutputData(0, outputData);
}