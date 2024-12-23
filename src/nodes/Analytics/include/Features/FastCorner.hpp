#pragma once
#include "Analytics.h"

class FastCorner : public BaseNodeModel
{
    Q_OBJECT
public:
    FastCorner();
    static const NodeDesc desc;
    ~FastCorner() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _threshold = 10;
    bool _nonMaxSuppression = true;
};