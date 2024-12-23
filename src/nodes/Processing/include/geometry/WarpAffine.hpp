#pragma once
#include "processing.h"

class WarpAffine : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    WarpAffine();
    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

private:
    cv::Mat _out;
};
