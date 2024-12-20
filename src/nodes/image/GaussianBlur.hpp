#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QWidget>
#include <QJsonObject>
#include <QVariant>
#include <QJsonValue>
#include <QJsonArray>
#include <opencv2/opencv.hpp>
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"

using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using namespace Flow;

class Gaussianblur : public BaseNodeModel
{
    Q_OBJECT

public:
    Gaussianblur();
    static const NodeDesc desc;

protected:
    void process() override;

    //int _kernelSize = 3;
    //double _sigma = 1.0;
    bool _normalize = false;
    cv::Mat _input;
    cv::Mat _output;
};
