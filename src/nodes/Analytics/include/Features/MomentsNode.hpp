#pragma once
#include "Analytics.h"

class MomentsNode : public BaseNodeModel
{
    Q_OBJECT
public:
    MomentsNode();
    static const NodeDesc desc;
    ~MomentsNode() override = default;

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    bool _binaryImage = false;
    int _method = 0;
};
