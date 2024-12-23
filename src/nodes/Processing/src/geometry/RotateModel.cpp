#include "RotateModel.hpp"

#include "Common.hpp"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QDoubleSpinBox>
#include <opencv2/imgproc.hpp>

using namespace QtNodes;
using namespace Flow;

const NodeDesc RotateModel::desc = 
{
    "RotateModel",
    "Rotate",
    CAT_PROC_GEOMETRY,
    { PortDesc::In("input", NodeType::Mat) },
    { PortDesc::Out("rotated", NodeType::Mat) },
    {
        ParamDesc::makeDouble("angle", "Rotation Angle (deg)", 0.0)
            .range(-180.0, 180.0)
    }
};

RotateModel::RotateModel()
    :BaseNodeModel(RotateModel::desc)
{
    // Initialize with default values
    _angle = parameterValue("angle").toDouble();
}

void RotateModel::setInData(std::shared_ptr<NodeData> data, PortIndex const port)
{
    if (port == 0) {
        _inputData = std::dynamic_pointer_cast<MatNodeData>(data);
        if (_inputData) {
            process();
        } else {
            _outputData = nullptr;
            Q_EMIT dataUpdated(0);
        }
    }
}

std::shared_ptr<NodeData> RotateModel::outData(PortIndex const port)
{
    if (port == 0) {
        return _outputData;
    }
    return nullptr;
}

QWidget* RotateModel::embeddedWidget()
{
    if (!_widget) {
        _widget = new QWidget();
        auto* layout = new QVBoxLayout(_widget);

        // Create angle control
        auto* angleControlLayout = new QHBoxLayout();
        auto* angleLabel = new QLabel("Angle:");
        _angleSpinBox = new QDoubleSpinBox();
        _angleSpinBox->setRange(-180.0, 180.0);
        _angleSpinBox->setSingleStep(1.0);
        _angleSpinBox->setValue(_angle);
        _angleSpinBox->setSuffix("°");

        _angleSlider = new QSlider(Qt::Horizontal);
        _angleSlider->setRange(-180, 180);
        _angleSlider->setValue(static_cast<int>(_angle));

        angleControlLayout->addWidget(angleLabel);
        angleControlLayout->addWidget(_angleSpinBox);
        angleControlLayout->addWidget(_angleSlider);

        layout->addLayout(angleControlLayout);

        // Connect slider and spinbox
        connect(_angleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
                [this](double value) {
                    _angleSlider->setValue(static_cast<int>(value));
                    setParameter("angle", value);
                    _angle = value;
                    if (_inputData) {
                        process();
                    }
                });

        connect(_angleSlider, &QSlider::valueChanged, 
                [this](int value) {
                    _angleSpinBox->setValue(value);
                });

        layout->addStretch();  // Add stretch to keep controls at the top
    }

    return _widget;
}

void RotateModel::process()
{
    if (!_inputData || _inputData->mat().empty()) {
        _outputData = nullptr;
        Q_EMIT dataUpdated(0);
        return;
    }

    cv::Mat inputMat = _inputData->mat();
    cv::Mat rotatedMat;

    // Get rotation matrix
    cv::Point2f center(inputMat.cols / 2.0, inputMat.rows / 2.0);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, _angle, 1.0);

    // Apply the transformation
    cv::warpAffine(inputMat, rotatedMat, rotationMatrix, inputMat.size());

    // Store result
    _outputData = std::make_shared<MatNodeData>(rotatedMat);

    // Emit data updated signal
    Q_EMIT dataUpdated(0);
}

void RotateModel::updateWidgetValues()
{
    if (_angleSpinBox) {
        _angleSpinBox->setValue(_angle);
    }
    if (_angleSlider) {
        _angleSlider->setValue(static_cast<int>(_angle));
    }
}