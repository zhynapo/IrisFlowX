#pragma once
#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class BoolNodeData : public NodeData
{
public:
    BoolNodeData() {}
    BoolNodeData(bool v) : _value(v) {}

    NodeDataType type() const override {
        return NodeDataType{"bool", "Boolean"};
    }

    bool value() const { return _value; }

private:
    bool _value = false;
};
