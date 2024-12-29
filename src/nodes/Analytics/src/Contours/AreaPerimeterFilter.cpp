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
    "Area/Perimeter Filter",
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
        
        ParamDesc::makeBool("enable_perimeter", "Enable Perimeter Filter", false),
        ParamDesc::makeDouble("min_perimeter", "Min Perimeter", 0.0).range(0.0, 100000.0),
        ParamDesc::makeDouble("max_perimeter", "Max Perimeter", 10000.0).range(0.0, 100000.0)
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
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["area_label"] = "Area: 0.000";
        _paramValues["perimeter_label"] = "Perimeter: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["area_label"] = "Area: 0.000";
        _paramValues["perimeter_label"] = "Perimeter: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    
    // Get parameters
    bool enableArea = parameterValue("enable_area").toBool();
    int minArea = parameterValue("min_area").toInt();
    int maxArea = parameterValue("max_area").toInt();
    
    bool enablePerimeter = parameterValue("enable_perimeter").toBool();
    double minPerimeter = parameterValue("min_perimeter").toDouble();
    double maxPerimeter = parameterValue("max_perimeter").toDouble();
    
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
        
        // Perimeter filter
        if (passesAllFilters && enablePerimeter) {
            double perimeter = cv::arcLength(contour, true);
            if (perimeter < minPerimeter || perimeter > maxPerimeter) {
                passesAllFilters = false;
            }
        }
        
        if (passesAllFilters) {
            filteredContours.push_back(contour);
        }
    }
    
    if (filteredContours.empty()) {
        // No contours passed all filters, return null
        setOutputData(0, nullptr);
        return;
    }
    
    // Create contour data with all filtered contours
    auto outputData = std::make_shared<ContoursNodeData>(filteredContours);
    
    // Output the results
    setOutputData(0, outputData);
}
