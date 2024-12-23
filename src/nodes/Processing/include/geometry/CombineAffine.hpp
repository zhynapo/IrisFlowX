#pragma once
#include "processing.h"

class CombineAffine : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    CombineAffine();
    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

private:
    cv::Mat _M;
};