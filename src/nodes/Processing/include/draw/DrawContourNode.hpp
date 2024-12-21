#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QColor>
#include "processing.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortType;
using QtNodes::PortIndex;

class DrawContourNode : public BaseNodeModel
{
    Q_OBJECT
public:
    DrawContourNode();

    //QWidget* embeddedWidget() override;
    static const NodeDesc desc;
    
private:
    void process() override;

private:
    QWidget* _widget = nullptr;
    ColorPickerWidget* _colorPicker = nullptr;

};
