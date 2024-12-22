#pragma once
#include "processing.h"

class AlphaTest : public BaseNodeModel
{
    Q_OBJECT
public:
    AlphaTest();
    static const NodeDesc desc;
    ~AlphaTest() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _alpha = 0.5;
};
