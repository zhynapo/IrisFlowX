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
#include "SiftFeature.hpp"
#include "Nodetype.hpp"

const NodeDesc SiftFeature::desc =
{
    "SiftFeature",
    "SIFT Feature Detector",
    CAT_ANALY_FEATURES,

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat),
        PortDesc::Out("features", NodeType::VisionGeometry)
    },

    // parameters
    {
        ParamDesc::makeInt("nFeatures", "Number of Features", 0).range(0, 10000),
        ParamDesc::makeInt("nOctaveLayers", "Octave Layers", 3).range(1, 10),
        ParamDesc::makeDouble("contrastThreshold", "Contrast Threshold", 0.04).range(0.01, 1.0),
        ParamDesc::makeDouble("edgeThreshold", "Edge Threshold", 10.0).range(1.0, 50.0),
        ParamDesc::makeDouble("sigma", "Sigma", 1.6).range(0.5, 3.0)
    }
};

SiftFeature::SiftFeature()
    : BaseNodeModel(SiftFeature::desc)
{
}

void SiftFeature::process()
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
    int nFeatures = parameterValue("nFeatures").toInt();
    int nOctaveLayers = parameterValue("nOctaveLayers").toInt();
    double contrastThreshold = parameterValue("contrastThreshold").toDouble();
    double edgeThreshold = parameterValue("edgeThreshold").toDouble();
    double sigma = parameterValue("sigma").toDouble();

    // Create SIFT detector with parameters
    // Note: SIFT implementation might require opencv_contrib
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create(
        nFeatures,
        nOctaveLayers,
        contrastThreshold,
        edgeThreshold,
        sigma
    );

    // Detect keypoints using SIFT
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    sift->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

    // Create output image with features marked
    cv::Mat output;
    if (_input.channels() == 1) {
        cv::cvtColor(_input, output, cv::COLOR_GRAY2BGR);
    } else {
        output = _input.clone();
    }

    // Extract feature points and draw them with orientation
    std::vector<cv::Point> features;
    for (const auto& kp : keypoints) {
        // Draw circle at keypoint location
        cv::circle(output, kp.pt, cvRound(kp.size * 0.25), cv::Scalar(0, 255, 255), 1); // Yellow circles for SIFT features
        
        // Draw line to indicate orientation
        if (kp.angle >= 0) {
            cv::Point orientEndPoint(
                cvRound(kp.pt.x + kp.size * cos(kp.angle * CV_PI / 180.0)),
                cvRound(kp.pt.y + kp.size * sin(kp.angle * CV_PI / 180.0))
            );
            cv::line(output, kp.pt, orientEndPoint, cv::Scalar(0, 255, 255), 1);
        }
        
        features.push_back(kp.pt);
    }

    // Create geometry data for the detected features
    auto geomData = std::make_shared<VisionGeometryNodeData>();
    
    if (!features.empty()) {
        geomData->typeValue = VisionGeometryNodeData::Type::Contour;
        geomData->valid = true;
        geomData->contour = features;  // Store all detected features as a contour
        
        // Set center as the average of all feature positions
        cv::Point2f sum(0, 0);
        for (const auto& feature : features) {
            sum.x += feature.x;
            sum.y += feature.y;
        }
        geomData->center = cv::Point2f(sum.x / features.size(), sum.y / features.size());
    } else {
        geomData->typeValue = VisionGeometryNodeData::Type::None;
        geomData->valid = false;
    }

    setOutputData(0, std::make_shared<MatNodeData>(output));
    setOutputData(1, geomData);
}