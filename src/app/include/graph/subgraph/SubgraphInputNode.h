// SubgraphInputNode.h
#pragma once
#include "model/BaseNodeModel.hpp"

using namespace QtNodes;
using namespace Flow;

class SubgraphInputNode : public BaseNodeModel
{
public:
    SubgraphInputNode(QString typeName = "");

    QString name() const override { return "SubgraphInput"; }
    QString caption() const override { return "Subgraph Input"; }
    bool captionVisible() const override { return true; }

    QWidget * embeddedWidget() override { return nullptr; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex index) const override;

    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setType(QString const& t);
    void setValue(std::shared_ptr<NodeData> d);

    QJsonObject save() const override;
    void load(QJsonObject const& p) override;

private:
    QString _typeName;
    std::shared_ptr<NodeData> _value;
};