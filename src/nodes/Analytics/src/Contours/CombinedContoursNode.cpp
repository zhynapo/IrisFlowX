#include "CombinedContoursNode.hpp"
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

static QVector<QString> MODE_NAMES = {"RETR_EXTERNAL", "RETR_LIST", "RETR_CCOMP", "RETR_TREE", "RETR_FLOODFILL"};
static QVector<int> MODE_VALUES = {cv::RETR_EXTERNAL, cv::RETR_LIST, cv::RETR_CCOMP, cv::RETR_TREE, cv::RETR_FLOODFILL};

static QVector<QString> METHOD_NAMES = {"CHAIN_APPROX_NONE", "CHAIN_APPROX_SIMPLE", "CHAIN_APPROX_TC89_L1", "CHAIN_APPROX_TC89_KCOS"};
static QVector<int> METHOD_VALUES = {cv::CHAIN_APPROX_NONE, cv::CHAIN_APPROX_SIMPLE, cv::CHAIN_APPROX_TC89_L1, cv::CHAIN_APPROX_TC89_KCOS};

const NodeDesc CombinedContoursNode::desc = {
    "Combined Contours",
    "CombinedContoursNode",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("image", NodeType::Mat)
    },
    {
        PortDesc::Out("contours", NodeType::Contours)
    },
    {
        // FindContours parameters
        ParamDesc::makeCombo("mode", "Retrieval Mode", MODE_NAMES, MODE_VALUES, cv::RETR_EXTERNAL),
        ParamDesc::makeCombo("method", "Approximation Method", METHOD_NAMES, METHOD_VALUES, cv::CHAIN_APPROX_SIMPLE),
        
        // ContoursSelect parameters
        ParamDesc::makeInt("filter_by_area", "Filter by Area", 1).range(0, 1),
        ParamDesc::makeInt("min_area", "Min Area", 10).range(0, 1000000),
        ParamDesc::makeInt("max_area", "Max Area", 100000).range(0, 1000000),
        ParamDesc::makeInt("filter_by_length", "Filter by Length", 0).range(0, 1),
        ParamDesc::makeInt("min_length", "Min Length", 10).range(0, 100000),
        ParamDesc::makeInt("max_length", "Max Length", 10000).range(0, 100000),
        ParamDesc::makeInt("filter_by_circularity", "Filter by Circularity", 0).range(0, 1),
        ParamDesc::makeDouble("min_circularity", "Min Circularity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_circularity", "Max Circularity", 1.0).range(0.0, 1.0)
    }
};

CombinedContoursNode::CombinedContoursNode()
    : BaseNodeModel(desc)
{
}

void CombinedContoursNode::process()
{
    // Get image input
    auto imgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    
    if (!imgData || imgData->mat().empty()) {
        setOutputData(0, nullptr);
        return;
    }
    
    cv::Mat inputMat = imgData->mat();
    
    // Ensure input image is binary (CV_8UC1) as required by findContours
    cv::Mat processedMat;
    if (inputMat.channels() > 1) {
        // Convert to grayscale if multi-channel
        cv::cvtColor(inputMat, processedMat, cv::COLOR_BGR2GRAY);
    } else {
        // Just copy if already single channel
        processedMat = inputMat.clone();
    }
    
    // Ensure data type is CV_8UC1
    if (processedMat.type() != CV_8UC1) {
        processedMat.convertTo(processedMat, CV_8UC1);
    }
    
    // Get parameters
    _mode = parameterValue("mode").toInt();
    _method = parameterValue("method").toInt();
    _filterByArea = parameterValue("filter_by_area").toInt();
    _minArea = parameterValue("min_area").toInt();
    _maxArea = parameterValue("max_area").toInt();
    _filterByLength = parameterValue("filter_by_length").toInt();
    _minLength = parameterValue("min_length").toInt();
    _maxLength = parameterValue("max_length").toInt();
    _filterByCircularity = parameterValue("filter_by_circularity").toInt();
    _minCircularity = parameterValue("min_circularity").toDouble();
    _maxCircularity = parameterValue("max_circularity").toDouble();
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat hierarchy;
    
    cv::findContours(processedMat, contours, hierarchy, static_cast<cv::RetrievalModes>(_mode), static_cast<cv::ContourApproximationModes>(_method));
    
    // Apply filters
    std::vector<std::vector<cv::Point>> filteredContours;
    
    for (const auto& contour : contours) {
        bool keep = true;
        
        // Check area filter
        if (_filterByArea) {
            double area = cv::contourArea(contour);
            if (area < _minArea || area > _maxArea) {
                keep = false;
            }
        }
        
        // Check length filter
        if (keep && _filterByLength) {
            double length = cv::arcLength(contour, true); // true for closed contours
            if (length < _minLength || length > _maxLength) {
                keep = false;
            }
        }
        
        // Check circularity filter
        if (keep && _filterByCircularity) {
            double area = cv::contourArea(contour);
            double perimeter = cv::arcLength(contour, true);
            
            if (perimeter > 0) {
                double circularity = 4 * CV_PI * area / (perimeter * perimeter);
                
                if (circularity < _minCircularity || circularity > _maxCircularity) {
                    keep = false;
                }
            }
        }
        
        if (keep) {
            filteredContours.push_back(contour);
        }
    }
    
    // Create contour data
    //auto outputData = std::make_shared<VisionGeometryNodeData>(VisionGeometryNodeData::Type::Contour);
    //outputData->setContours(filteredContours);
    auto outputData = std::make_shared<ContoursNodeData>(filteredContours);

    // Output the results
    setOutputData(0, outputData);
}