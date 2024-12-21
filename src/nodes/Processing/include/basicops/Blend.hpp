#pragma once
#include "processing.h"

class Blend : public BaseNodeModel
{
    Q_OBJECT
public:
    Blend();
    static const NodeDesc desc;
    ~Blend() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _alpha = 0.5;
    bool _closed = true;
};
