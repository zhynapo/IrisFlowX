#pragma once
#include "Analytics.h"

class MinEnclosingCircle : public BaseNodeModel
{
    Q_OBJECT
public:
    MinEnclosingCircle();
    static const NodeDesc desc;
    ~MinEnclosingCircle() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _epsilon = 1.0;
    bool _closed = true;
};
