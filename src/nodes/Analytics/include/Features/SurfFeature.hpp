#pragma once
#include "Analytics.h"

// Check if xfeatures2d is available
#if defined(HAVE_OPENCV_XFEATURES2D) && defined(OPENCV_ENABLE_NONFREE)
#include <opencv2/xfeatures2d.hpp>
#endif
#include <opencv2/features2d.hpp>

class SurfFeature : public BaseNodeModel
{
    Q_OBJECT
public:
    SurfFeature();
    static const NodeDesc desc;
    ~SurfFeature() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _hessianThreshold = 100.0;
    int _nOctaves = 4;
    int _nOctaveLayers = 3;
    bool _extended = false;
    bool _upright = false;
};