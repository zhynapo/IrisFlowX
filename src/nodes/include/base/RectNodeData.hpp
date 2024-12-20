
#pragma once
#include <opencv2/opencv.hpp>
#include <QtNodes/NodeData>

using QtNodes::NodeDataType;

class RectNodeData : public QtNodes::NodeData
{
public:
    RectNodeData() {}
    RectNodeData(const cv::Rect& r) : _rect(r) {}

    NodeDataType type() const override {
        return NodeDataType{"rect", "Rect"};
    }

    cv::Rect rect() const { return _rect; }

private:
    cv::Rect _rect;
};