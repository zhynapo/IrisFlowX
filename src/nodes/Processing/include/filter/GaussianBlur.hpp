#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QWidget>
#include <QJsonObject>
#include <QVariant>
#include <QJsonValue>
#include <QJsonArray>
#include "processing.h"

using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;

class GaussianBlur : public BaseNodeModel
{
    Q_OBJECT

public:
    GaussianBlur();
    static const NodeDesc desc;

protected:
    void process() override;

    //int _kernelSize = 3;
    //double _sigma = 1.0;
    bool _normalize = false;
    cv::Mat _input;
    cv::Mat _output;
};
