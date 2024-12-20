#pragma once
#include <nodes/NodeDataModel>
#include <QJsonObject>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class JsonNodeData : public NodeData
{
public:
    JsonNodeData() {}
    JsonNodeData(const QJsonObject& obj) : _obj(obj) {}

    NodeDataType type() const override {
        return NodeDataType{"json", "Json"};
    }

    QJsonObject object() const { return _obj; }

private:
    QJsonObject _obj;
};
