#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QLabel>

#include "PixmapData.hpp"

class ImageShowModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    ImageShowModel();
    ~ImageShowModel() override = default;

    QString caption() const override { return "Image Viewer"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageShowModel"; }

    QWidget* embeddedWidget() override { return _label; }

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QLabel* _label = nullptr;
    std::shared_ptr<QtNodes::NodeData> _nodeData;
};