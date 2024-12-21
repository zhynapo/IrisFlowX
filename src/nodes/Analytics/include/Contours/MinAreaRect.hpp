#pragma once
#include "Analytics.h"

class MinAreaRect : public BaseNodeModel
{
    Q_OBJECT
public:
    MinAreaRect();
    static const NodeDesc desc;
    ~MinAreaRect() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _epsilon = 1.0;
    bool _closed = true;
};
