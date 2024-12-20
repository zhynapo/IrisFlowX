// SubgraphInputNode.cpp
#include "graph/subgraph/SubgraphInputNode.h"

SubgraphInputNode::SubgraphInputNode(QString typeName)
    : _typeName(typeName.isEmpty() ? "mat" : typeName)
{
    _outputData.resize(1);
    initTimer();
}

unsigned int SubgraphInputNode::nPorts(PortType portType) const
{
    return (portType == PortType::Out ? 1 : 0);
}

NodeDataType SubgraphInputNode::dataType(PortType, PortIndex) const
{
    return {_typeName, _typeName};
}

std::shared_ptr<NodeData> SubgraphInputNode::outData(PortIndex)
{
    return _value;
}

void SubgraphInputNode::setType(QString const& t)
{
    _typeName = t;
}

void SubgraphInputNode::setValue(std::shared_ptr<NodeData> d)
{
    qDebug() << "[setInData]" << caption()
        << "setValue"  ;

    _value = d;
    update();
    //Q_EMIT dataUpdated(0);   // 触发内部节点链执行
}

QJsonObject SubgraphInputNode::save() const
{
    QJsonObject obj = BaseNodeModel::save();
    obj["typeName"] = _typeName;
    return obj;
}

void SubgraphInputNode::load(QJsonObject const& p)
{
    BaseNodeModel::load(p);
    if (p.contains("typeName"))
        _typeName = p["typeName"].toString();
}
