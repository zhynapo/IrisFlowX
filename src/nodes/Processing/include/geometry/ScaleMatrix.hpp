#pragma once
#include "processing.h"

class ScaleMatrix : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    ScaleMatrix();
    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;
private:
    cv::Mat _M;
};