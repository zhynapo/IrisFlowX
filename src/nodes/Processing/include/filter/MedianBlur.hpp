#pragma once
#include "processing.h"

class MedianBlur : public BaseNodeModel
{
    Q_OBJECT
public:
    MedianBlur();
    static const NodeDesc desc;
    ~MedianBlur() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _ksize = 3;
};
