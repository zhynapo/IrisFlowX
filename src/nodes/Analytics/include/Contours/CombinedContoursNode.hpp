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

class CombinedContoursNode : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit CombinedContoursNode();
    virtual ~CombinedContoursNode() = default;

    static const NodeDesc desc;

private:
    void process();

private:
    int _mode = 3; // RETR_EXTERNAL as default
    int _method = 2; // CHAIN_APPROX_SIMPLE as default
    int _minArea = 10;
    int _maxArea = 100000;
    int _minLength = 10;
    int _maxLength = 10000;
    double _minCircularity = 0.0;
    double _maxCircularity = 1.0;
    int _filterByArea = 1;
    int _filterByLength = 0;
    int _filterByCircularity = 0;
};