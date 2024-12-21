#pragma once
#include "Analytics.h"

class PolygonApprox : public BaseNodeModel
{
    Q_OBJECT
public:
    PolygonApprox();
    static const NodeDesc desc;
    ~PolygonApprox() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _epsilon = 1.0;
    bool _closed = true;
};
