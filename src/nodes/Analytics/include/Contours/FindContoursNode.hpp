#pragma once
#include "Analytics.h"
#include <opencv2/opencv.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortType;
using QtNodes::PortIndex;

class FindContoursNode : public BaseNodeModel
{
    Q_OBJECT
public:
    FindContoursNode();
    static const NodeDesc desc;

protected:
    void process() override;

private:
    std::vector<std::vector<cv::Point>> _contours;
    
};
