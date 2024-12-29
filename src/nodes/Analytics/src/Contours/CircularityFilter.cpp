#include "CircularityFilter.hpp"
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
#include <algorithm> // 添加算法头文件用于min_element和max_element
#include <QDebug>

const NodeDesc CircularityFilter::desc = {
    "Circularity Filter",
    "CircularityFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        ParamDesc::makeDouble("min_circularity", "Min Circularity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_circularity", "Max Circularity", 1.0).range(0.0, 1.0)
    }
};

CircularityFilter::CircularityFilter()
    : BaseNodeModel(desc)
{
}

void CircularityFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["circularity_label"] = "Circularity: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["circularity_label"] = "Circularity: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    // Get parameters with clamping to fixed valid range [0, 1]
    double minCircularity = std::clamp(parameterValue("min_circularity").toDouble(), 0.0, 1.0);
    double maxCircularity = std::clamp(parameterValue("max_circularity").toDouble(), 0.0, 1.0);

    // Ensure min <= max
    if (minCircularity > maxCircularity) {
        std::swap(minCircularity, maxCircularity);
        setParameter("min_circularity", minCircularity);
        setParameter("max_circularity", maxCircularity);
    }
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply circularity filter to each contour
    for (const auto& contour : inputContours) {
        bool passesFilter = true;
        
        // Circularity filter
        double area = cv::contourArea(contour);
        double perimeter = cv::arcLength(contour, true);
        if (perimeter > 0) {
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity < minCircularity || circularity > maxCircularity) {
                passesFilter = false;
            }
        } else {
            passesFilter = false; // Invalid contour
        }
        
        if (passesFilter) {
            filteredContours.push_back(contour);
        }
    }
    
    if (filteredContours.empty()) {
        // No contours passed the filter, return null
        setOutputData(0, nullptr);
        return;
    }
    
    // Create contour data with all filtered contours
    auto outputData = std::make_shared<ContoursNodeData>(filteredContours);
    
    // Output the results
    setOutputData(0, outputData);
}

