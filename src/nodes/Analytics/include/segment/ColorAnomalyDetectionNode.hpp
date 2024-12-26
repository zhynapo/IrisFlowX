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

class ColorAnomalyDetectionNode : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit ColorAnomalyDetectionNode();
    virtual ~ColorAnomalyDetectionNode() = default;

    static const NodeDesc desc;

private:
    void process();

private:
    // Parameters for the algorithm
    int _borderMargin = 10;
    int _blurKSize = 5;
    double _kSigma = 2.5;
    double _thrMin = 15.0;
    double _thrMax = 60.0;
    int _morphSize = 3;
    int _minArea = 50;
    double _lowerH = 0.0;
    double _lowerS = 0.0;
    double _lowerV = 0.0;
    double _upperH = 179.0;
    double _upperS = 255.0;
    double _upperV = 255.0;
};