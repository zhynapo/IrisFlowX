#include "ExtentFilter.hpp"
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

const NodeDesc ExtentFilter::desc = {
    "Extent Filter",
    "ExtentFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        ParamDesc::makeDouble("min_extent", "Min Extent", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_extent", "Max Extent", 1.0).range(0.0, 1.0),
        
        // Index parameter for selecting from filtered results
        ParamDesc::makeInt("index", "Contour Index", 0).range(0, 100),
        
        // Parameters to display contour metrics using labels
        ParamDesc::makeLabel("extent_label", "Extent", "0.0")
    }
};

ExtentFilter::ExtentFilter()
    : BaseNodeModel(desc)
{
}

void ExtentFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["extent_label"] = "Extent: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["extent_label"] = "Extent: 0.000";
        setParamRange("index", 0, 0);  // 设置为无效范围
        setParamEnabled("index", false);  // 禁用索引参数
        return;
    }
    
    // 计算所有轮廓的范围，用于动态调整参数范围
    std::vector<double> extents;
    
    for (const auto& contour : inputContours) {
        cv::Rect boundingRect = cv::boundingRect(contour);
        double boundingArea = boundingRect.width * boundingRect.height;
        if (boundingArea > 0) {
            double extent = cv::contourArea(contour) / boundingArea;
            extents.push_back(extent);
        }
    }
    
    // 动态调整参数范围
    if (!extents.empty()) {
        double minExtent = *std::min_element(extents.begin(), extents.end());
        double maxExtent = *std::max_element(extents.begin(), extents.end());
        
        // 调整范围参数范围
        setParamRange("min_extent", minExtent, maxExtent);
        setParamRange("max_extent", minExtent, maxExtent);
        
        // 获取当前参数值并确保在新范围内
        double currentMinExtent = std::max(minExtent, std::min(parameterValue("min_extent").toDouble(), maxExtent));
        double currentMaxExtent = std::max(currentMinExtent, std::min(parameterValue("max_extent").toDouble(), maxExtent));
        
        // 更新参数值（这将在_process中不会触发更新）
        setParameter("min_extent", currentMinExtent);
        setParameter("max_extent", currentMaxExtent);
    }
    
    // Get parameters
    double minExtent = parameterValue("min_extent").toDouble();
    double maxExtent = parameterValue("max_extent").toDouble();
    
    int index = parameterValue("index").toInt();
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply filters to each contour
    for (const auto& contour : inputContours) {
        bool passesAllFilters = true;
        
        // Extent filter
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
        
        if (passesAllFilters) {
            filteredContours.push_back(contour);
        }
    }
    
    // Update parameter ranges based on filtered contour count
    int n = filteredContours.size();
    setParamRange("index", 0, n > 0 ? n - 1 : 0);  // 如果没有轮廓，设置为0
    setParamEnabled("index", n > 0);  // 仅在有轮廓时启用
    
    std::vector<std::vector<cv::Point>> outputContours;
    
    if (filteredContours.empty()) {
        // No contours passed all filters, return null
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["extent_label"] = "Extent: 0.000";
        return;
    }
    
    std::vector<cv::Point> selectedContour;
    if (index >= 0 && index < n) {
        selectedContour = filteredContours[index];
        outputContours.push_back(selectedContour);
    } else if (n > 0) {
        // If index is out of range but there are contours, default to the first contour
        selectedContour = filteredContours[0];
        outputContours.push_back(selectedContour);
    } else {
        // If there are no filtered contours, return null
        setOutputData(0, nullptr);
        // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
        _paramValues["extent_label"] = "Extent: 0.000";
        return;
    }
    
    // Calculate metric for the selected contour
    cv::Rect boundingRect = cv::boundingRect(selectedContour);
    double boundingArea = boundingRect.width * boundingRect.height;
    double area = cv::contourArea(selectedContour);
    double extent = 0.0;
    if (boundingArea > 0) {
        extent = area / boundingArea;
    }
    
    // 直接修改内部参数值，而不是调用setParameter，以避免触发更新
    _paramValues["extent_label"] = QString("Extent: %1").arg(extent, 0, 'f', 3);
    
    // Create contour data
    auto outputData = std::make_shared<ContoursNodeData>(outputContours);
    
    // Output the results
    setOutputData(0, outputData);
}