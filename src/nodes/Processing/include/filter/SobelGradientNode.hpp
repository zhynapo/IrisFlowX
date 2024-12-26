#pragma once
#include "processing.h"

class SobelGradientNode : public BaseNodeModel
{
    Q_OBJECT

public:
    SobelGradientNode();
    static const NodeDesc desc;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _dx = 1;      // Order of derivative in x direction
    int _dy = 0;      // Order of derivative in y direction
    int _kSize = 3;   // Aperture size
    double _scale = 1.0;   // Scale factor
    double _delta = 0.0;   // Delta value
    int _borderType = cv::BORDER_DEFAULT;  // Border type
    
    // Helper function to validate orders against kernel size
    int validateOrder(int order, int kSize);
};