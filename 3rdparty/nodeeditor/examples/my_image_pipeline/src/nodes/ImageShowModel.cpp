#include "ImageShowModel.hpp"
#include "ImagePreviewDialog.hpp"
#include <QtWidgets/QMenu>

using namespace QtNodes;

ImageShowModel::ImageShowModel()
    : _label(new QLabel("Image will appear here"))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);

    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);
}

unsigned int ImageShowModel::nPorts(PortType portType) const
{
    switch (portType) {
    case PortType::In:  return 1;
    case PortType::Out: return 1; // 透传
    default:            return 0;
    }
}

NodeDataType ImageShowModel::dataType(PortType, PortIndex) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> ImageShowModel::outData(PortIndex)
{
    return _nodeData;
}

void ImageShowModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    _nodeData = nodeData;

    if (_nodeData) {
        if (auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData)) {
            int w = _label->width();
            int h = _label->height();
            _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else {
        _label->setPixmap(QPixmap());
    }

    Q_EMIT dataUpdated(0);
}

bool ImageShowModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            if (auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData)) {
                _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick) {
            if (auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData)) {
                if (d && !d->pixmap().isNull()) {
                    auto* dlg = new ImagePreviewDialog(_label);
                    dlg->setAttribute(Qt::WA_DeleteOnClose);
                    dlg->setPixmap(d->pixmap());
                    dlg->show();
                    return true;
                }
            }
        }
        else if (event->type() == QEvent::ContextMenu) {
            if (auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData)) {
                if (d && !d->pixmap().isNull()) {
                    QMenu menu;
                    QAction* actPreview = menu.addAction(tr("Preview Large..."));
                    QAction* chosen = menu.exec(QCursor::pos());
                    if (chosen == actPreview) {
                        auto* dlg = new ImagePreviewDialog(_label);
                        dlg->setAttribute(Qt::WA_DeleteOnClose);
                        dlg->setPixmap(d->pixmap());
                        dlg->show();
                    }
                    return true;
                }
            }
        }
    }
    return false;
}