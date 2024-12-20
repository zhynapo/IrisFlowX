#include "ImagePreviewDialog.hpp"
#include <QtGui/QPainter>

ImagePreviewDialog::ImagePreviewDialog(QWidget* parent)
    : QDialog(parent)
{
    _scene = new QGraphicsScene(this);
    _view = new ZoomGraphicsView(_scene);
    _view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(_view);

    resize(900, 650);
    setWindowTitle(tr("Image Preview"));
}

void ImagePreviewDialog::setPixmap(const QPixmap& pm)
{
    _scene->clear();
    auto* item = _scene->addPixmap(pm);
    item->setTransformationMode(Qt::SmoothTransformation);
    _scene->setSceneRect(pm.rect());
    _view->fitInView(item, Qt::KeepAspectRatio);
}