#pragma once
#include <opencv2/opencv.hpp>
#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class PointNodeData : public NodeData
{
public:
    PointNodeData() {}
    PointNodeData(const cv::Point2f& p) : _pt(p) {}

    NodeDataType type() const override {
        return NodeDataType{"point", "Point"};
    }

    cv::Point2f value() const { return _pt; }

private:
    cv::Point2f _pt;
};