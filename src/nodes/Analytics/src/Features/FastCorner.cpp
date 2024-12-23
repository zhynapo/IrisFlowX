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
#include "FastCorner.hpp"
#include "Nodetype.hpp"

const NodeDesc FastCorner::desc =
{
    "FastCorner",
    "FAST Corner Detector",
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
        ParamDesc::makeInt("threshold", "Threshold", 10).range(1, 100),
        ParamDesc::makeBool("nonMaxSuppression", "Non-Max Suppression", true)
    }
};

FastCorner::FastCorner()
    : BaseNodeModel(FastCorner::desc)
{
}

void FastCorner::process()
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
    int threshold = parameterValue("threshold").toInt();
    bool nonMaxSuppression = parameterValue("nonMaxSuppression").toBool();

    // Detect corners using FAST algorithm
    std::vector<cv::KeyPoint> keypoints;
    cv::FAST(gray, keypoints, threshold, nonMaxSuppression);

    // Create output image with corners marked
    cv::Mat output;
    if (_input.channels() == 1) {
        cv::cvtColor(_input, output, cv::COLOR_GRAY2BGR);
    } else {
        output = _input.clone();
    }

    // Extract corner points and draw them
    std::vector<cv::Point> corners;
    for (const auto& kp : keypoints) {
        cv::circle(output, kp.pt, 3, cv::Scalar(0, 0, 255), 1);
        corners.push_back(kp.pt);
    }

    // Create geometry data for the detected corners
    auto geomData = std::make_shared<VisionGeometryNodeData>();
    
    if (!corners.empty()) {
        geomData->typeValue = VisionGeometryNodeData::Type::Contour;
        geomData->valid = true;
        geomData->contour = corners;  // Store all detected corners as a contour
        
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