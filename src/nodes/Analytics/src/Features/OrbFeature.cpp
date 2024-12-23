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
#include "OrbFeature.hpp"
#include "Nodetype.hpp"

const NodeDesc OrbFeature::desc =
{
    "OrbFeature",
    "ORB Feature Detector",
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
        ParamDesc::makeInt("nFeatures", "Number of Features", 500).range(1, 10000),
        ParamDesc::makeDouble("scaleFactor", "Scale Factor", 1.2).range(1.0, 2.0),
        ParamDesc::makeInt("nLevels", "Number of Levels", 8).range(1, 16),
        ParamDesc::makeInt("edgeThreshold", "Edge Threshold", 31).range(1, 100),
        ParamDesc::makeInt("firstLevel", "First Level", 0).range(0, 5),
        ParamDesc::makeInt("wtaK", "WTA K", 2).range(2, 4),
        ParamDesc::makeInt("scoreType", "Score Type (0-Harris, 1-FAST)", 0).range(0, 1),
        ParamDesc::makeInt("patchSize", "Patch Size", 31).range(1, 100),
        ParamDesc::makeInt("fastThreshold", "FAST Threshold", 20).range(1, 100)
    }
};

OrbFeature::OrbFeature()
    : BaseNodeModel(OrbFeature::desc)
{
}

void OrbFeature::process()
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
    double scaleFactor = parameterValue("scaleFactor").toDouble();
    int nLevels = parameterValue("nLevels").toInt();
    int edgeThreshold = parameterValue("edgeThreshold").toInt();
    int firstLevel = parameterValue("firstLevel").toInt();
    int wtaK = parameterValue("wtaK").toInt();
    int scoreType = parameterValue("scoreType").toInt();
    int patchSize = parameterValue("patchSize").toInt();
    int fastThreshold = parameterValue("fastThreshold").toInt();

    // Create ORB detector with parameters
    cv::Ptr<cv::ORB> orb = cv::ORB::create(
        nFeatures,
        scaleFactor,
        nLevels,
        edgeThreshold,
        firstLevel,
        wtaK,
        scoreType == 0 ? cv::ORB::HARRIS_SCORE : cv::ORB::FAST_SCORE,
        patchSize,
        fastThreshold
    );

    // Detect keypoints using ORB
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    orb->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

    // Create output image with features marked
    cv::Mat output;
    if (_input.channels() == 1) {
        cv::cvtColor(_input, output, cv::COLOR_GRAY2BGR);
    } else {
        output = _input.clone();
    }

    // Extract feature points and draw them
    std::vector<cv::Point> features;
    for (const auto& kp : keypoints) {
        cv::circle(output, kp.pt, 3, cv::Scalar(255, 165, 0), 1); // Orange circles for ORB features
        cv::line(output, 
                 cv::Point(kp.pt.x - 2, kp.pt.y - 2), 
                 cv::Point(kp.pt.x + 2, kp.pt.y + 2), 
                 cv::Scalar(255, 165, 0), 1); // Plus sign to indicate orientation
        cv::line(output, 
                 cv::Point(kp.pt.x + 2, kp.pt.y - 2), 
                 cv::Point(kp.pt.x - 2, kp.pt.y + 2), 
                 cv::Scalar(255, 165, 0), 1);
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