#pragma once
#include "processing.h"

class RotateMatrix : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    RotateMatrix();

    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

private:
    cv::Mat _M;
};