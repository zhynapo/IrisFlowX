#pragma once
#include "Analytics.h"

class OrbFeature : public BaseNodeModel
{
    Q_OBJECT
public:
    OrbFeature();
    static const NodeDesc desc;
    ~OrbFeature() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _nFeatures = 500;
    float _scaleFactor = 1.2f;
    int _nLevels = 8;
    int _edgeThreshold = 31;
    int _firstLevel = 0;
    int _wtaK = 2;
    int _scoreType = 0; // 0 for Harris, 1 for FAST
    int _patchSize = 31;
    int _fastThreshold = 20;
};