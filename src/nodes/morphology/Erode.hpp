#pragma once
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"
#include <opencv2/opencv.hpp>

using namespace Flow;

class Erode : public BaseNodeModel
{
    Q_OBJECT
public:
    Erode();
    static const NodeDesc desc;
    ~Erode() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _ksize = 3;
    int _iterations = 1;
};
