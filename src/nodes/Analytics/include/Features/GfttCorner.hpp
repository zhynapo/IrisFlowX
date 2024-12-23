#pragma once
#include "Analytics.h"

class GfttCorner : public BaseNodeModel
{
    Q_OBJECT
public:
    GfttCorner();
    static const NodeDesc desc;
    ~GfttCorner() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _maxCorners = 1000;
    double _qualityLevel = 0.01;
    double _minDistance = 1.0;
    int _blockSize = 3;
    bool _useHarris = false;
    double _harrisK = 0.04;
};