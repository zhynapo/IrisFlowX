// SubgraphOutputNode.h
#pragma once
#include "model/BaseNodeModel.hpp"

using namespace QtNodes;
using namespace Flow;


class SubgraphOutputNode : public BaseNodeModel
{
public:
    SubgraphOutputNode(QString typeName ="");

    QString name() const override { return "SubgraphOutput"; }
    QString caption() const override { return "Subgraph Output"; }
    bool captionVisible() const override { return true; }

    QWidget * embeddedWidget() override { return nullptr; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex index) const override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex port) override;
    std::shared_ptr<NodeData> outData(PortIndex) override { 
        return nullptr; 
    }

    void setType(QString const& t);
    std::shared_ptr<NodeData> value() const;

    QJsonObject save() const override;
    void load(QJsonObject const& p) override;

private:
    QString _typeName;
    std::shared_ptr<NodeData> _value;
};