#include "GaussianModel.hpp"
#include <QPixmap>
#include <QImage>
#include <QtCore/QDebug>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace QtNodes;

static cv::Mat QPixmapToMat(const QPixmap &pixmap)
{
    QImage img = pixmap.toImage().convertToFormat(QImage::Format_RGB888);
    return cv::Mat(img.height(), img.width(),
                   CV_8UC3,
                   const_cast<uchar*>(img.bits()),
                   img.bytesPerLine()).clone();
}

static QPixmap MatToQPixmap(const cv::Mat &mat)
{
    cv::Mat rgb;
    if (mat.channels() == 3)
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    else
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);

    QImage image(rgb.data, rgb.cols, rgb.rows,
                 static_cast<int>(rgb.step), QImage::Format_RGB888);

    return QPixmap::fromImage(image.copy());
}

GaussianModel::GaussianModel()
{
    _widget = new QWidget();
    auto* layout = new QVBoxLayout(_widget);

    auto* label = new QLabel("Kernel Size");
    _slider = new QSlider(Qt::Horizontal);
    _slider->setRange(1, 40); // 1->3, 2->5, ...
    _slider->setValue(2);

    layout->addWidget(label);
    layout->addWidget(_slider);

    connect(_slider, &QSlider::valueChanged,
            this, &GaussianModel::onSliderChanged);

    // 初始化 _kernelSize
    onSliderChanged(_slider->value());
}

void GaussianModel::applyGaussian()
{
    if (_input.empty())
        return;

    int k = _kernelSize;
    if (k < 3) k = 3;
    if ((k & 1) == 0) k += 1;

    // sigma=0 由 OpenCV 根据 k 自动推导
    cv::GaussianBlur(_input, _cached, cv::Size(k, k), 0.0, 0.0, cv::BORDER_REPLICATE);
}

static int toKernel(int sliderValue)
{
    return sliderValue * 2 + 1; // 1->3, 2->5, 3->7, ...
}

void GaussianModel::onSliderChanged(int value)
{
    _kernelSize = toKernel(value);
    applyGaussian();
    Q_EMIT dataUpdated(0);
}

unsigned int GaussianModel::nPorts(PortType portType) const
{
    switch (portType)
    {
        case PortType::In:  return 1;
        case PortType::Out: return 1;
        default:            return 0;
    }
}

QtNodes::NodeDataType GaussianModel::dataType(PortType, PortIndex) const
{
    return PixmapData().type();
}

void GaussianModel::setInData(std::shared_ptr<NodeData> data, PortIndex)
{
    auto pixmapData = std::dynamic_pointer_cast<PixmapData>(data);
    if (!pixmapData)
        return;

    _input = QPixmapToMat(pixmapData->pixmap());
    applyGaussian();
    Q_EMIT dataUpdated(0);
}

std::shared_ptr<NodeData> GaussianModel::outData(PortIndex)
{
    if (_cached.empty())
        return nullptr;

    return std::make_shared<PixmapData>(MatToQPixmap(_cached));
}