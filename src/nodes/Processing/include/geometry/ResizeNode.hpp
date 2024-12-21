
#pragma once
#include "processing.h"

class ResizeNode : public BaseNodeModel
{
    Q_OBJECT
public:
    ResizeNode();
    static const NodeDesc desc;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    // Parameters
    double _scaleX = 1.0;
    double _scaleY = 1.0;
    int _interpolation = cv::INTER_LINEAR;

};
