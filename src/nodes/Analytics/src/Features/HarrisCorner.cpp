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
#include "HarrisCorner.hpp"
#include "Nodetype.hpp"

const NodeDesc HarrisCorner::desc =
{
    "HarrisCorner",
    "Harris Corner Detector",
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
        ParamDesc::makeInt("blockSize", "blockSize", 2).range(2, 30),
        ParamDesc::makeInt("kSize", "kSize", 3).range(3, 31).setStep(2),
        ParamDesc::makeDouble("k", "k", 0.04).range(0.01, 0.2),
        ParamDesc::makeDouble("threshold", "threshold", 0.01).range(0.001, 1.0)
    }
};

HarrisCorner::HarrisCorner()
    : BaseNodeModel(HarrisCorner::desc)
{
}

void HarrisCorner::process()
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
    int blockSize = parameterValue("blockSize").toInt();
    int kSize = parameterValue("kSize").toInt();
    double k = parameterValue("k").toDouble();
    double threshold = parameterValue("threshold").toDouble();

    // Apply Harris corner detection
    cv::Mat dst;
    cv::cornerHarris(gray, dst, blockSize, kSize, k, cv::BORDER_DEFAULT);

    // Normalize the result matrix for display
    cv::Mat norm_dst;
    cv::normalize(dst, norm_dst, 0, 255, cv::NORM_MINMAX, CV_32F);

    // Threshold to identify strong corners
    cv::Mat mask;
    cv::threshold(norm_dst, mask, threshold * 255, 255, cv::THRESH_BINARY);

    // Convert mask to proper type for accessing pixels
    cv::Mat mask_8u;
    mask.convertTo(mask_8u, CV_8UC1);

    // Vector to store detected corner points
    std::vector<cv::Point> corners;
    
    // Find corner coordinates
    for (int i = 0; i < mask_8u.rows; i++) {
        for (int j = 0; j < mask_8u.cols; j++) {
            if (mask_8u.at<uchar>(i, j) > 128) {  // Check if pixel is white (255)
                corners.push_back(cv::Point(j, i));
            }
        }
    }

    // Create output image with corners marked
    cv::Mat output;
    if (_input.channels() == 1) {
        cv::cvtColor(_input, output, cv::COLOR_GRAY2BGR);
    } else {
        output = _input.clone();
    }

    // Draw all detected corners on the output image
    for (const auto& corner : corners) {
        cv::circle(output, corner, 3, cv::Scalar(0, 0, 255), 1);
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