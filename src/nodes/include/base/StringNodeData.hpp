#pragma once
#include <nodes/NodeDataModel>
#include <QString>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class StringNodeData : public NodeData
{
public:
    StringNodeData() {}
    StringNodeData(const QString& s) : _str(s) {}

    NodeDataType type() const override {
        return NodeDataType{"string", "String"};
    }

    QString string() const { return _str; }

private:
    QString _str;
};
