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
#include <exception>
#include <QDebug>
#include "SurfFeature.hpp"
#include "Nodetype.hpp"

// Always include xfeatures2d, but handle runtime availability
#include <opencv2/xfeatures2d.hpp>

const NodeDesc SurfFeature::desc =
{
    "SurfFeature",
    "SURF Feature Detector",
    "Analytics/Features",

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
        ParamDesc::makeDouble("hessianThreshold", "Hessian Threshold", 100.0).range(1.0, 1000.0),
        ParamDesc::makeInt("nOctaves", "Number of Octaves", 4).range(1, 6),
        ParamDesc::makeInt("nOctaveLayers", "Octave Layers", 3).range(1, 10),
        ParamDesc::makeBool("extended", "Extended Descriptors", false),
        ParamDesc::makeBool("upright", "Upright Features", false)
    }
};

SurfFeature::SurfFeature()
    : BaseNodeModel(SurfFeature::desc)
{
}

void SurfFeature::process()
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
    double hessianThreshold = parameterValue("hessianThreshold").toDouble();
    int nOctaves = parameterValue("nOctaves").toInt();
    int nOctaveLayers = parameterValue("nOctaveLayers").toInt();
    bool extended = parameterValue("extended").toBool();
    bool upright = parameterValue("upright").toBool();

    // Try to create SURF detector with parameters, catch the exception if not available
    try {
        // Attempt to create SURF detector
        cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(
            hessianThreshold,
            nOctaves,
            nOctaveLayers,
            extended,
            upright
        );

        // Detect keypoints using SURF
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        surf->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

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
            cv::circle(output, kp.pt, cvRound(kp.size * 0.25), cv::Scalar(255, 0, 255), 1); // Magenta circles for SURF features
            
            // Draw line to indicate orientation
            if (!upright && kp.angle >= 0) {
                cv::Point orientEndPoint(
                    cvRound(kp.pt.x + kp.size * cos(kp.angle * CV_PI / 180.0)),
                    cvRound(kp.pt.y + kp.size * sin(kp.angle * CV_PI / 180.0))
                );
                cv::line(output, kp.pt, orientEndPoint, cv::Scalar(255, 0, 255), 1);
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
    catch (const cv::Exception& e) {
        // SURF is not available due to patent restrictions or missing module
        qDebug() << "SURF Feature Detector is not available:" << e.what();
        setOutputData(0, nullptr);
        setOutputData(1, nullptr);
    }
    catch (...) {
        // Handle any other unexpected exceptions
        qDebug() << "SURF Feature Detector is not available due to configuration restrictions";
        setOutputData(0, nullptr);
        setOutputData(1, nullptr);
    }
}