#include "ImagePreviewWidget.hpp"

ImagePreviewWidget::ImagePreviewWidget(QWidget* parent)
    : QWidget(parent),
    _scene(new QGraphicsScene(this)),
    _view(new ZoomGraphicsView)
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(_view);
    layout->setContentsMargins(0, 0, 0, 0);

    _view->setScene(_scene);
    _view->setDragMode(QGraphicsView::ScrollHandDrag);
    _view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void ImagePreviewWidget::setPixmap(const QPixmap& pm)
{
    _scene->clear();
    if (!pm.isNull()) {
        _scene->addPixmap(pm);
        _scene->setSceneRect(pm.rect());
        _view->fitInView(pm.rect(), Qt::KeepAspectRatio);
    }
}
