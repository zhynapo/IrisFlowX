#pragma once
#include "Analytics.h"

class HarrisCorner : public BaseNodeModel
{
    Q_OBJECT
public:
    HarrisCorner();
    static const NodeDesc desc;
    ~HarrisCorner() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _blockSize = 2;
    int _kSize = 3;
    double _k = 0.04;
    double _threshold = 0.01;
};