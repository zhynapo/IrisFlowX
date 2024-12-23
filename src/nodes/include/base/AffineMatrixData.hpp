#pragma once
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>

using QtNodes::NodeData;

class AffineMatrixData : public QtNodes::NodeData
{
public:
    cv::Mat mat; // 2x3 CV_64F

    AffineMatrixData() {
        mat = cv::Mat::eye(2, 3, CV_64F);
    }
    AffineMatrixData(const cv::Mat& m) {
        m.copyTo(mat);
    }

    QtNodes::NodeDataType type() const override {
        return { "affinematrix", "Affine Matrix 2x3" };
    }
};