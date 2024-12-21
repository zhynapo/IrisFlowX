#pragma once
#include "processing.h"

class DrawRotatedRect : public BaseNodeModel
{
    Q_OBJECT
public:
    DrawRotatedRect();
    static const NodeDesc desc;
    ~DrawRotatedRect() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    double _epsilon = 1.0;
    bool _closed = true;
};
