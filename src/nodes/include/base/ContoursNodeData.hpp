#pragma once
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>
#include <vector>

class ContoursNodeData : public QtNodes::NodeData
{
public:
    using Contours = std::vector<std::vector<cv::Point>>;

    ContoursNodeData() = default;
    explicit ContoursNodeData(const Contours& c) : _contours(c) {}

    QtNodes::NodeDataType type() const override { return {"contours", "Contours"}; }

    const Contours& value() const { return _contours; }

private:
    Contours _contours;
};

