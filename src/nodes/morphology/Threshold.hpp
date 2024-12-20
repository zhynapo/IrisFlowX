#pragma once
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"
#include <opencv2/opencv.hpp>

using namespace Flow;

class Threshold : public BaseNodeModel
{
    Q_OBJECT
public:
    Threshold();
    static const NodeDesc desc;
    ~Threshold() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _thresh = 128;
    int _maxval = 255;
    int _type = 0;
};
