#include "AreaPerimeterFilter.hpp"
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

const NodeDesc AreaPerimeterFilter::desc = {
    "Area & Perimeter Filter",
    "AreaPerimeterFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        // Boolean parameters to enable/disable filters
        ParamDesc::makeBool("enable_area", "Enable Area Filter", false),
        ParamDesc::makeInt("min_area", "Min Area", 0).range(0, 1000000),
        ParamDesc::makeInt("max_area", "Max Area", 100000).range(0, 1000000),
        
        ParamDesc::makeBool("enable_length", "Enable Length Filter", false),
        ParamDesc::makeInt("min_length", "Min Length", 0).range(0, 100000),
        ParamDesc::makeInt("max_length", "Max Length", 10000).range(0, 100000),
        
        // Index parameter for selecting from filtered results
        ParamDesc::makeInt("index", "Contour Index", 0).range(0, 100),
        
        // Parameters to display contour metrics using labels
        ParamDesc::makeLabel("area_label", "Area", "0.0"),
        ParamDesc::makeLabel("perimeter_label", "Perimeter", "0.0")
    }
};

AreaPerimeterFilter::AreaPerimeterFilter()
    : BaseNodeModel(desc)
{
}

void AreaPerimeterFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        setParameter("area_label", "Area: 0.0");
        setParameter("perimeter_label", "Perimeter: 0.0");
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        setParameter("area_label", "Area: 0.0");
        setParameter("perimeter_label", "Perimeter: 0.0");
        return;
    }
    
    // Get parameters
    bool enableArea = parameterValue("enable_area").toBool();
    int minArea = parameterValue("min_area").toInt();
    int maxArea = parameterValue("max_area").toInt();
    
    bool enableLength = parameterValue("enable_length").toBool();
    int minLength = parameterValue("min_length").toInt();
    int maxLength = parameterValue("max_length").toInt();
    
    int index = parameterValue("index").toInt();
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply filters to each contour
    for (const auto& contour : inputContours) {
        bool passesAllFilters = true;
        
        // Area filter
        if (passesAllFilters && enableArea) {
            double area = cv::contourArea(contour);
            if (area < minArea || area > maxArea) {
                passesAllFilters = false;
            }
        }
        
        // Length filter
        if (passesAllFilters && enableLength) {
            double perimeter = cv::arcLength(contour, true);
            if (perimeter < minLength || perimeter > maxLength) {
                passesAllFilters = false;
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
        setParameter("area_label", "Area: 0.0");
        setParameter("perimeter_label", "Perimeter: 0.0");
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
    
    // Update the parameters with the calculated metrics
    setParameter("area_label", QString("Area: %1").arg(area, 0, 'f', 2));
    setParameter("perimeter_label", QString("Perimeter: %1").arg(perimeter, 0, 'f', 2));
    
    // Create contour data
    auto outputData = std::make_shared<ContoursNodeData>(outputContours);
    
    // Output the results
    setOutputData(0, outputData);
}