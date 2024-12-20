// SubgraphOutputNode.cpp
#include "graph/subgraph/SubgraphOutputNode.h"

SubgraphOutputNode::SubgraphOutputNode(QString typeName)
    : _typeName(typeName.isEmpty() ? "mat" : typeName)
{
    _inputData.resize(1);
    _outputData.resize(1);
    initTimer();
}

unsigned int SubgraphOutputNode::nPorts(PortType portType) const
{
    return (portType == PortType::In ? 1 : 0);
}

NodeDataType SubgraphOutputNode::dataType(PortType, PortIndex) const
{
    return {_typeName, _typeName};
}

void SubgraphOutputNode::setInData(std::shared_ptr<NodeData> data, PortIndex)
{
    _value = data;
    update();
    //emit dataUpdated(0);
}

void SubgraphOutputNode::setType(QString const& t)
{
    _typeName = t;
}

std::shared_ptr<NodeData> SubgraphOutputNode::value() const
{
    return _value;
}

QJsonObject SubgraphOutputNode::save() const
{
    QJsonObject obj = BaseNodeModel::save();
    obj["typeName"] = _typeName;
    return obj;
}

void SubgraphOutputNode::load(QJsonObject const& p)
{
    BaseNodeModel::load(p);
    if (p.contains("typeName"))
        _typeName = p["typeName"].toString();
}