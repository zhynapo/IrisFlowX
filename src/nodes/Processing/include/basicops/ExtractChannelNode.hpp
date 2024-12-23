#pragma once
#include "processing.h"

class ExtractChannelNode : public BaseNodeModel
{
    Q_OBJECT

public:
    ExtractChannelNode();
    static const NodeDesc desc;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _channel = 0; // 默认提取第0通道
};