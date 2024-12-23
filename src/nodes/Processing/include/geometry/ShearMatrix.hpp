#pragma once
#include "processing.h"

class ShearMatrix : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    ShearMatrix();
    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;
private:
    cv::Mat _M;
};