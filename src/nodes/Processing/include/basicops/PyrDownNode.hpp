#pragma once
#include "processing.h"

class PyrDownNode : public BaseNodeModel
{
    Q_OBJECT

public:
    PyrDownNode();
    static const NodeDesc desc;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _borderType = cv::BORDER_DEFAULT;
};