#pragma once
#include "processing.h"

class MorphologyNode : public BaseNodeModel
{
public:
    MorphologyNode();
    static const NodeDesc desc;

protected:
    void process() override;

    cv::Mat applyMorphPerChannel(const cv::Mat& src, int op, int shape, int ksize);

};