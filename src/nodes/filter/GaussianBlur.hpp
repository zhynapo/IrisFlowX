#pragma once
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"
#include <opencv2/opencv.hpp>

using namespace Flow;

class GaussianBlur : public BaseNodeModel
{
    Q_OBJECT
public:
    GaussianBlur();
    static const NodeDesc desc;
    ~GaussianBlur() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _ksize = 3;
};
