#include "ContoursSelectNode.hpp"
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

const NodeDesc ContoursSelectNode::desc = {
    "Select Contour",
    "ContoursSelectNode",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("selected_contour", NodeType::Contours)
    },
    {
        // Index parameter for selecting from input contours
        ParamDesc::makeInt("index", "Contour Index", 0).range(0, 100),
        
        // Parameters to display contour metrics using labels
        ParamDesc::makeLabel("area_label", "Area", "0.0"),
        ParamDesc::makeLabel("perimeter_label", "Perimeter", "0.0"),
        ParamDesc::makeLabel("circularity_label", "Circularity", "0.0"),
        ParamDesc::makeLabel("solidity_label", "Solidity", "0.0"),
        ParamDesc::makeLabel("extent_label", "Extent", "0.0"),
        ParamDesc::makeLabel("eccentricity_label", "Eccentricity", "0.0")
    }
};

ContoursSelectNode::ContoursSelectNode()
    : BaseNodeModel(desc)
{
    setParameter("area_label", "0.0");
    setParameter("perimeter_label", "0.0");
    setParameter("circularity_label", "0.000");
    setParameter("solidity_label", "0.000");
    setParameter("extent_label", "0.000");
    setParameter("eccentricity_label", "0.000");
}

void ContoursSelectNode::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        setOutputData(0, nullptr);
        // 现在可以直接使用setParameter，系统会自动处理更新逻辑
        setParameter("area_label", "0.0");
        setParameter("perimeter_label", "0.0");
        setParameter("circularity_label", "0.000");
        setParameter("solidity_label", "0.000");
        setParameter("extent_label", "0.000");
        setParameter("eccentricity_label", "0.000");
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        // 现在可以直接使用setParameter，系统会自动处理更新逻辑
        setParameter("area_label", "0.0");
        setParameter("perimeter_label", "0.0");
        setParameter("circularity_label", "0.000");
        setParameter("solidity_label", "0.000");
        setParameter("extent_label", "0.000");
        setParameter("eccentricity_label", "0.000");
        return;
    }
    
    // Get parameters
    int index = parameterValue("index").toInt();
    
    // Update parameter ranges based on input contour count
    int n = inputContours.size();
    setParamRange("index", 0, n - 1);
    setParamEnabled("index", n > 0);
    
    // Select the contour at the specified index
    std::vector<std::vector<cv::Point>> outputContours;
    
    if (index >= 0 && index < n) {
        outputContours.push_back(inputContours[index]);
    } else {
        // If index is out of range, default to the first contour
        outputContours.push_back(inputContours[0]);
    }
    
    // Calculate metrics for the selected contour
    auto selectedContour = outputContours[0];
    
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
    
    double eccentricity = 0.0;
    if (selectedContour.size() >= 5) {
        cv::RotatedRect ellipse = cv::fitEllipse(selectedContour);
        double semiMajorAxis = std::max(ellipse.size.width, ellipse.size.height) / 2.0;
        double semiMinorAxis = std::min(ellipse.size.width, ellipse.size.height) / 2.0;
        
        if (semiMajorAxis > semiMinorAxis) {
            eccentricity = sqrt(1 - (semiMinorAxis * semiMinorAxis) / (semiMajorAxis * semiMajorAxis));
        }
    }
    
    // 现在可以直接使用setParameter，系统会自动处理更新逻辑
    
    setParameter("area_label", QString("%1").arg(area, 0, 'f', 2));
    setParameter("perimeter_label", QString("%1").arg(perimeter, 0, 'f', 2));
    setParameter("circularity_label", QString("%1").arg(circularity, 0, 'f', 3));
    setParameter("solidity_label", QString("%1").arg(solidity, 0, 'f', 3));
    setParameter("extent_label", QString("%1").arg(extent, 0, 'f', 3));
    setParameter("eccentricity_label", QString("%1").arg(eccentricity, 0, 'f', 3));
    
    // Create contour data
    auto outputData = std::make_shared<ContoursNodeData>(outputContours);
    
    // Output the results
    setOutputData(0, outputData);
}