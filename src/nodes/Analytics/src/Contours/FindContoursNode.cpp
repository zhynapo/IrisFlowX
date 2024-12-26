#include "FindContoursNode.hpp"
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

const NodeDesc FindContoursNode::desc = {
    "Find Contours",
    "FindContoursNode",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("image", NodeType::Mat)
    },
    {
        PortDesc::Out("contours", NodeType::Contours)
        // Removed hierarchy output since it's not a visualizable image
    },
    {
        ParamDesc::makeCombo("mode", "Retrieval Mode", MODE_NAMES, MODE_VALUES, cv::RETR_EXTERNAL),
        ParamDesc::makeCombo("method", "Approximation Method", METHOD_NAMES, METHOD_VALUES, cv::CHAIN_APPROX_SIMPLE)
    }
};

FindContoursNode::FindContoursNode()
    : BaseNodeModel(desc)
{
}

void FindContoursNode::process()
{
    // Get image input
    auto imgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    
    if (!imgData || imgData->mat().empty()) {
        setOutputData(0, nullptr);
        //setOutputData(1, nullptr);
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
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat hierarchy;
    
    cv::findContours(processedMat, contours, hierarchy, static_cast<cv::RetrievalModes>(_mode), static_cast<cv::ContourApproximationModes>(_method));
    
    // Create contour data
    //auto contourData = std::make_shared<VisionGeometryNodeData>(VisionGeometryNodeData::Type::Contour);
    //contourData->setContours(contours);
    auto outputData = std::make_shared<ContoursNodeData>(contours);

    // Output the results
    setOutputData(0, outputData);
}