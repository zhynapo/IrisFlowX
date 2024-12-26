#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QWidget>
#include <QJsonObject>
#include <QVariant>
#include <QJsonValue>
#include <QJsonArray>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>

#include "Analytics.h"

using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;

class EccentricityFilter : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit EccentricityFilter();
    virtual ~EccentricityFilter() = default;

    static const NodeDesc desc;

private:
    void process() override;
};