#pragma once
#include "processing.h"

class TranslateMatrix : public Flow::BaseNodeModel
{
public:
    static const NodeDesc desc;
    TranslateMatrix();

    void process() override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

private:
    cv::Mat _M;
};