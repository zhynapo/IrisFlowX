// MatNodeData.hpp
#pragma once
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>

class MatNodeData : public QtNodes::NodeData
{
public:
    MatNodeData() {}
    MatNodeData(const cv::Mat &m) : _mat(m) {}

    QtNodes::NodeDataType type() const override {
        return {"mat", "cv::Mat"};
    }

    cv::Mat const &mat() const { return _mat; }
    cv::Mat &mat() { return _mat; }

private:
    cv::Mat _mat;
};
