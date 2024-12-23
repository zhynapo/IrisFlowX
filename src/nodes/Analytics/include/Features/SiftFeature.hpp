#pragma once
#include "Analytics.h"

class SiftFeature : public BaseNodeModel
{
    Q_OBJECT
public:
    SiftFeature();
    static const NodeDesc desc;
    ~SiftFeature() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _nFeatures = 0; // 0 means no limit
    int _nOctaveLayers = 3;
    double _contrastThreshold = 0.04;
    double _edgeThreshold = 10.0;
    double _sigma = 1.6;
};