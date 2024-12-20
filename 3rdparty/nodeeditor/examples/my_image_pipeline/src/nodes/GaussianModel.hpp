#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>

#include <opencv2/core.hpp>

#include "PixmapData.hpp"

class GaussianModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    GaussianModel();
    ~GaussianModel() override = default;

    QString caption() const override { return "Gaussian Blur"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "GaussianModel"; }

    QWidget* embeddedWidget() override { return _widget; }

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override;

private slots:
    void onSliderChanged(int value);

private:
    void applyGaussian();

    QWidget* _widget = nullptr;
    QSlider* _slider = nullptr;
    int _kernelSize = 5;

    cv::Mat _input;
    cv::Mat _cached;
};