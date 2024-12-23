#pragma once

#include <iostream>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "processing.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;

class RotateModel : public BaseNodeModel {
    Q_OBJECT

public:
    RotateModel();
    static const NodeDesc desc;

public:
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex const port) override;
    QWidget* embeddedWidget() override;

protected:
    void process() override;

private:
    void updateWidgetValues();

private:
    std::shared_ptr<MatNodeData> _inputData;
    std::shared_ptr<MatNodeData> _outputData;
    
    // UI Components
    QWidget* _widget = nullptr;
    QDoubleSpinBox* _angleSpinBox = nullptr;
    QSlider* _angleSlider = nullptr;
    
    // Parameters
    double _angle = 0.0;  // Rotation angle in degrees
};