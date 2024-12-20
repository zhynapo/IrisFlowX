#pragma once
#include <opencv2/opencv.hpp>
#include <QtNodes/NodeData>
#include <vector>

using QtNodes::NodeDataType;

class PointsNodeData : public QtNodes::NodeData
{
public:
    PointsNodeData() {}
    PointsNodeData(const std::vector<cv::Point>& pts) : _pts(pts) {}

    NodeDataType type() const override {
        return NodeDataType{"points", "Points"};
    }

    std::vector<cv::Point> value() const { return _pts; }

private:
    std::vector<cv::Point> _pts;
};
