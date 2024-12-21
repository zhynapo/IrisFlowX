#pragma once
#include "processing.h"

class PixelWiseOp : public BaseNodeModel
{
    Q_OBJECT
public:
    PixelWiseOp();
    static const NodeDesc desc;
    ~PixelWiseOp() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _mode = 0;
};
