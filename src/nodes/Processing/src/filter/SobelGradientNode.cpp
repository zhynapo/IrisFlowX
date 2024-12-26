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
#include "SobelGradientNode.hpp"
#include "Nodetype.hpp"

const NodeDesc SobelGradientNode::desc =
{
    "SobelGradient",
    "Sobel Gradient",
    "Processing/Filter",

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeInt("dx", "Order X", 1).range(0, 6),
        ParamDesc::makeInt("dy", "Order Y", 0).range(0, 6),
        ParamDesc::makeInt("kSize", "Kernel Size", 3).range(1, 31).setStep(2),
        ParamDesc::makeDouble("scale", "Scale", 1.0).range(0.1, 10.0),
        ParamDesc::makeDouble("delta", "Delta", 0.0).range(-10.0, 10.0),
        ParamDesc::makeCombo("borderType", "Border Type", 
            QVector<QString>{"Default", "Replicate", "Reflect", "Wrap", "Constant"},
            QVector<int>{cv::BORDER_DEFAULT, cv::BORDER_REPLICATE, cv::BORDER_REFLECT, cv::BORDER_WRAP, cv::BORDER_CONSTANT},
            cv::BORDER_DEFAULT)
    }
};

SobelGradientNode::SobelGradientNode()
    : BaseNodeModel(SobelGradientNode::desc)
{
}

int SobelGradientNode::validateOrder(int order, int kSize) {
    // OpenCV Sobel function allows: 
    // - For kSize=1: effectively uses Scharr kernel, order can be 0 or 1
    // - For kSize>=3 and odd: order should not exceed kSize
    // - In practice, meaningful derivatives are limited by kernel size
    
    int maxAllowedOrder = 0;
    
    if (kSize == 1) {
        // For kSize=1, OpenCV uses Scharr kernel internally, max order is 1
        maxAllowedOrder = 1;
    } else {
        // For standard Sobel kernels, order should not exceed kSize
        // However, meaningful derivatives are typically much lower
        // For practical purposes: order <= (kSize-1)/2 for meaningful results
        maxAllowedOrder = (kSize - 1) / 2;
    }
    
    // Return the minimum of current order and max allowed order
    return std::min(order, maxAllowedOrder);
}

void SobelGradientNode::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    if (_input.empty())
    {
        setOutputData(0, nullptr);
        return;
    }

    // Get parameters
    int dx = parameterValue("dx").toInt();
    int dy = parameterValue("dy").toInt();
    int kSize = parameterValue("kSize").toInt();
    double scale = parameterValue("scale").toDouble();
    double delta = parameterValue("delta").toDouble();
    int borderType = parameterValue("borderType").toInt();

    // Ensure kSize is odd and at least 1
    if (kSize % 2 == 0) {
        kSize += 1;  // Make it odd
    }
    if (kSize < 1) {
        kSize = 1;   // Minimum size is 1
    }

    // Calculate max allowed order based on kSize
    int maxOrder = (kSize == 1) ? 1 : (kSize - 1) / 2;
    
    // Update the range for dx and dy parameters
    setParamRange("dx", 0, maxOrder);
    setParamRange("dy", 0, maxOrder);
    
    // Validate dx and dy against kSize
    int validatedDx = validateOrder(dx, kSize);
    int validatedDy = validateOrder(dy, kSize);
    
    // Ensure that dx and dy are not both zero (which would result in no gradient)
    if (validatedDx == 0 && validatedDy == 0) {
        // If both are 0, default to dx=1 (horizontal gradient)
        validatedDx = 1;
    }
    
    // Convert to grayscale if necessary
    cv::Mat gray;
    if (_input.channels() > 1) {
        cv::cvtColor(_input, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = _input;
    }

    // Apply Sobel operator
    cv::Sobel(gray, _output, CV_16S, validatedDx, validatedDy, kSize, scale, delta, borderType);

    // Convert back to displayable format
    cv::convertScaleAbs(_output, _output);

    // Set the output
    setOutputData(0, std::make_shared<MatNodeData>(_output));
}