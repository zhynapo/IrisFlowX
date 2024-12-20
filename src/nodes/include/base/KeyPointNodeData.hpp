#pragma once
#include <opencv2/opencv.hpp>
#include <nodes/NodeDataModel>
#include <vector>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class KeyPointNodeData : public NodeData
{
public:
    KeyPointNodeData() {}
    KeyPointNodeData(const std::vector<cv::KeyPoint>& kp) : _kps(kp) {}

    NodeDataType type() const override {
        return NodeDataType{"keypoints", "KeyPoints"};
    }

    std::vector<cv::KeyPoint> keypoints() const { return _kps; }

private:
    std::vector<cv::KeyPoint> _kps;
};
