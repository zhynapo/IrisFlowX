#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDataModel>
#include <QLabel>
#include <QPixmap>

#include "PixmapData.hpp"

class ImageLoaderModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    ImageLoaderModel();
    ~ImageLoaderModel() override = default;

    QString caption() const override { return "Image Loader"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageLoaderModel"; }

    QWidget* embeddedWidget() override { return _label; }

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QLabel* _label = nullptr;
    QPixmap _pixmap;
};