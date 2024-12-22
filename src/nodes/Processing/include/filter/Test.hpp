#pragma once
#include "processing.h"

class Test : public BaseNodeModel
{
    Q_OBJECT
public:
    Test();
    static const NodeDesc desc;
    ~Test() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _thresh = 128;
    int _maxval = 255;
    int _type = 0;
};
