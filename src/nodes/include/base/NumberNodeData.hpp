#pragma once
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>

using QtNodes::NodeDataType;

class NumberNodeData : public QtNodes::NodeData
{
public:
    NumberNodeData() {}
    NumberNodeData(double v) : _value(v) {}

    NodeDataType type() const override {
        return NodeDataType{"number", "Number"};
    }

    double value() const { return _value; }

private:
    double _value = 0.0;
};
