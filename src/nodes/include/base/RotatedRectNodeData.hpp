#pragma once
#include <opencv2/opencv.hpp>
#include <QtNodes/NodeData>

using QtNodes::NodeDataType;

class RotatedRectNodeData : public QtNodes::NodeData
{
public:
    RotatedRectNodeData() {}
    RotatedRectNodeData(const cv::RotatedRect& r) : _rect(r) {}

    NodeDataType type() const override {
        return NodeDataType{"rotatedrect", "RotatedRect"};
    }

    cv::RotatedRect value() const { return _rect; }

private:
    cv::RotatedRect _rect;
};
