#include "ImageLoaderModel.hpp"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

using namespace QtNodes;

ImageLoaderModel::ImageLoaderModel()
    : _label(new QLabel("Double click to load image"))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);

    _label->setMinimumSize(200, 200);
    _label->setMaximumSize(500, 300);

    _label->installEventFilter(this);
}

unsigned int ImageLoaderModel::nPorts(PortType portType) const
{
    switch (portType) {
    case PortType::In:  return 0;
    case PortType::Out: return 1;
    default:            return 0;
    }
}

NodeDataType ImageLoaderModel::dataType(PortType, PortIndex) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> ImageLoaderModel::outData(PortIndex)
{
    return std::make_shared<PixmapData>(_pixmap);
}

bool ImageLoaderModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::MouseButtonDblClick) {
            QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                            tr("Open Image"),
                                                            QDir::homePath(),
                                                            tr("Image Files (*.png *.jpg *.bmp *.jpeg)"));
            if (!fileName.isEmpty()) {
                _pixmap = QPixmap(fileName);
                if (!_pixmap.isNull())
                    _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                Q_EMIT dataUpdated(0);
            }
            return true;
        }
        else if (event->type() == QEvent::Resize) {
            if (!_pixmap.isNull())
                _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else if (event->type() == QEvent::ContextMenu) {
            if (!_pixmap.isNull()) {
                QMenu menu;
                QAction* actClear = menu.addAction(tr("Clear"));
                QAction* chosen = menu.exec(QCursor::pos());
                if (chosen == actClear) {
                    _pixmap = QPixmap();
                    _label->setPixmap(QPixmap());
                    Q_EMIT dataUpdated(0);
                }
                return true;
            }
        }
    }
    return false;
}