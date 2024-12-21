#include "ImageShowModel.hpp"
#include "NodeType.hpp"

#include "Common.hpp"
#include <QtWidgets/QMenu>
#include <QtGui/QMouseEvent>

using namespace QtNodes;


const NodeDesc ImageShowModel::desc =
Flow::NodeDesc("ImageShowModel", "Image Viewer", CAT_PROC_IO)
.addInput("mat", NodeType::Mat)
.addOutput("mat", NodeType::Mat);

// --------------- 自动注册 ----------------
//REGISTER_NODE(ImageShowModel)

ImageShowModel::ImageShowModel()
    :BaseNodeModel(desc)
{
    
}

QWidget* ImageShowModel::embeddedWidget() 
{
    if (!_label)
    {

        _label = new QLabel("Image will appear here\n(Click to preview)");
        _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

        QFont f = _label->font();
        f.setBold(true);
        f.setItalic(true);
        _label->setFont(f);

        _label->setMinimumSize(200, 200);

        // 提高事件可达性
        _label->setFocusPolicy(Qt::StrongFocus);
        _label->setAttribute(Qt::WA_NoMousePropagation, true);

        _label->installEventFilter(this);
    }
    return _label;
}

void ImageShowModel::process()
{
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat mat = matData->mat();
    _label->setPixmap(MatToQPixmap(mat).scaled(320, 240, Qt::KeepAspectRatio));
    //----- 6. 设置输出 -----
    setOutputData(0, matData);
}

