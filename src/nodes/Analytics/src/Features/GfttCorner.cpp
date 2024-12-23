#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonValue>
#include "GfttCorner.hpp"
#include "Nodetype.hpp"

const NodeDesc GfttCorner::desc =
{
    "GfttCorner",
    "GFTT Corner Detector",
    CAT_ANALY_FEATURES,

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat),
        PortDesc::Out("corners", NodeType::VisionGeometry)
    },

    // parameters
    {
        ParamDesc::makeInt("maxCorners", "Max Corners", 1000).range(1, 5000),
        ParamDesc::makeDouble("qualityLevel", "Quality Level", 0.01).range(0.001, 0.1),
        ParamDesc::makeDouble("minDistance", "Min Distance", 1.0).range(0.1, 50.0),
        ParamDesc::makeInt("blockSize", "Block Size", 3).range(1, 10),
        ParamDesc::makeBool("useHarris", "Use Harris", false),
        ParamDesc::makeDouble("harrisK", "Harris K", 0.04).range(0.01, 0.2)
    }
};

GfttCorner::GfttCorner()
    : BaseNodeModel(GfttCorner::desc)
{
}

void GfttCorner::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        setOutputData(1, nullptr);
        return;
    }

    _input = matData->mat();
    
    if (_input.empty())
    {
        setOutputData(0, nullptr);
        setOutputData(1, nullptr);
        return;
    }

    cv::Mat gray;
    if (_input.channels() > 1)
        cv::cvtColor(_input, gray, cv::COLOR_BGR2GRAY);
    else
        gray = _input;

    // Get parameters
    int maxCorners = parameterValue("maxCorners").toInt();
    double qualityLevel = parameterValue("qualityLevel").toDouble();
    double minDistance = parameterValue("minDistance").toDouble();
    int blockSize = parameterValue("blockSize").toInt();
    bool useHarris = parameterValue("useHarris").toBool();
    double harrisK = parameterValue("harrisK").toDouble();

    // Detect corners using goodFeaturesToTrack algorithm
    std::vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(
        gray, 
        corners, 
        maxCorners, 
        qualityLevel, 
        minDistance, 
        cv::Mat(), 
        blockSize, 
        useHarris, 
        harrisK
    );

    // Create output image with corners marked
    cv::Mat output;
    if (_input.channels() == 1) {
        cv::cvtColor(_input, output, cv::COLOR_GRAY2BGR);
    } else {
        output = _input.clone();
    }

    // Draw all detected corners on the output image
    for (const auto& corner : corners) {
        cv::circle(output, corner, 3, cv::Scalar(0, 255, 0), 1); // Green circles for GFTT corners
    }

    // Create geometry data for the detected corners
    auto geomData = std::make_shared<VisionGeometryNodeData>();
    
    if (!corners.empty()) {
        geomData->typeValue = VisionGeometryNodeData::Type::Contour;
        geomData->valid = true;
        
        // Convert cv::Point2f to cv::Point for contour storage
        geomData->contour.clear();
        for (const auto& corner : corners) {
            geomData->contour.push_back(cv::Point(corner.x, corner.y));
        }
        
        // Set center as the average of all corner positions
        cv::Point2f sum(0, 0);
        for (const auto& corner : corners) {
            sum.x += corner.x;
            sum.y += corner.y;
        }
        geomData->center = cv::Point2f(sum.x / corners.size(), sum.y / corners.size());
    } else {
        geomData->typeValue = VisionGeometryNodeData::Type::None;
        geomData->valid = false;
    }

    setOutputData(0, std::make_shared<MatNodeData>(output));
    setOutputData(1, geomData);
}