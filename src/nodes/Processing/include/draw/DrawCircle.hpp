#pragma once
#include "processing.h"

class DrawCircle : public BaseNodeModel
{
    Q_OBJECT
public:
    DrawCircle();
    static const NodeDesc desc;
    ~DrawCircle() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _epsilon = 1.0;
    bool _closed = true;
};
