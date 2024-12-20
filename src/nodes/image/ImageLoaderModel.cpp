
#include <core/IFlowPlugin.h>
#include "ImageLoaderModel.hpp"
#include "NodeType.hpp"

#include "Common.hpp"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

using namespace QtNodes;
using namespace Flow;

const NodeDesc ImageLoaderModel::desc = 
{
    "ImageLoaderModel",
    "Image Source",
    "IO",
    {  },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        ParamDesc::makeString("filepath", "File Path", "")   // ⭐ 新增
    }
};

// --------------- 自动注册 ----------------
//REGISTER_NODE(ImageLoaderModel)


ImageLoaderModel::ImageLoaderModel()
    :BaseNodeModel(ImageLoaderModel::desc)
{
    
}

QWidget* ImageLoaderModel::embeddedWidget()
{
    if (_widget) return _widget;

    _widget = new QWidget();
    auto* layout = new QVBoxLayout(_widget);

    _label = new QLabel("Click to preview\nRight-click to load image");
    layout->addWidget(_label);

    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);

    _label->setMinimumSize(200, 200);
    _label->setMaximumSize(500, 300);

    // 提高事件可达性（与官方示例一致）
    _label->setFocusPolicy(Qt::StrongFocus);
    _label->setAttribute(Qt::WA_NoMousePropagation, true);

    if (!_mat.empty()) {
        _label->setPixmap(MatToQPixmap(_mat).scaled(160, 120, Qt::KeepAspectRatio));
    }
    _label->installEventFilter(this);
    return _widget;
}

#if 0
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
    return MatNodeData().type();
}

#endif
std::shared_ptr<NodeData> ImageLoaderModel::outData(PortIndex)
{
    if (_mat.empty()) return nullptr;
    return std::make_shared<MatNodeData>(_mat);
}

void ImageLoaderModel::load(QJsonObject const& obj)
{
    BaseNodeModel::load(obj);   // ⭐ 调用基类处理参数恢复

    QString path = parameterValue("filepath").toString();

    if (!path.isEmpty()) {
        cv::Mat img = cv::imread(path.toStdString(), cv::IMREAD_UNCHANGED);
        if (!img.empty()) {
            _mat = img;
            if (_label)
                _label->setPixmap(MatToQPixmap(_mat).scaled(160, 120, Qt::KeepAspectRatio));

            Q_EMIT dataUpdated(0);
        }
    }
}

bool ImageLoaderModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        const int w = _label->width();
        const int h = _label->height();

        // 右键：加载/清空等
        if (event->type() == QEvent::ContextMenu) {
            QMenu menu;
            QAction* actLoad  = menu.addAction(tr("Load Image..."));
            QAction* actClear = !_mat.empty() ? menu.addAction(tr("Clear")) : nullptr;

            QAction* chosen = menu.exec(QCursor::pos());
            if (chosen == actLoad) {
                QString fileName = QFileDialog::getOpenFileName(_label,
                                                                tr("Open Image"),
                                                                QDir::homePath(),
                                                                tr("Image Files (*.png *.jpg *.bmp *.jpeg *.tif *.tiff)"));
                if (!fileName.isEmpty()) {
                    cv::Mat img = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
                    if (img.empty()) return false;

                    _mat = img;
                    _label->setPixmap(MatToQPixmap(_mat).scaled(160, 120, Qt::KeepAspectRatio));

                    setParameter("filepath", fileName);   // ⭐ 保存路径

                    Q_EMIT dataUpdated(0);
                }
                return true;
            } else if (chosen == actClear) {
                //_pixmap = QPixmap();
                _label->setPixmap(QPixmap());
                Q_EMIT dataUpdated(0);
                return true;
            }
        }
        else if (event->type() == QEvent::Resize) {
            if (!_mat.empty())
                _label->setPixmap(MatToQPixmap(_mat).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    return false;
}

