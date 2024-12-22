#pragma once
#include "processing.h"

class BilateralFilter : public BaseNodeModel
{
    Q_OBJECT
public:
    BilateralFilter();
    static const NodeDesc desc;
    ~BilateralFilter() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _ksize = 3;
};
