
#pragma once
#include "processing.h"

class ErodeNode : public BaseNodeModel
{
    Q_OBJECT
public:
    ErodeNode();
    static const NodeDesc desc;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    //int _kernelSize  = 3;
    //int _iterations  = 1;
    //int _borderType = cv::BORDER_CONSTANT;
};
