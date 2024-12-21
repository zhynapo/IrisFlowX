#include "ThresholdModel.hpp"
#include "NodeType.hpp"

#include "Common.hpp"
#include <QMetaObject>
#include <opencv2/imgproc.hpp>

using namespace QtNodes;

const NodeDesc ThresholdModel::desc =
{
    "Threshold",
    "Threshold",
    CAT_ANALY_THRESHOLD,
    { PortDesc::In("mat", NodeType::Mat) },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
    }
};

// --------------- 自动注册 ----------------
//REGISTER_NODE(ThresholdModel)

// -------------------- 构造 --------------------
ThresholdModel::ThresholdModel()
    :BaseNodeModel(desc)
{

}

QWidget* ThresholdModel::embeddedWidget()
{
    // 若已创建则直接返回
    if (_widget) return _widget;

    _widget = new QWidget();
    auto* layout = new QVBoxLayout(_widget);

    _combo = new QComboBox();
    _combo->addItems({
        "Binary",
        "BinaryInv",
        "Trunc",
        "ToZero",
        "AdaptiveMean",
        "AdaptiveGaussian"
    });

    _slider = new QSlider(Qt::Horizontal);
    _slider->setRange(0, 255);
    _slider->setValue(128);

    _blockSizeSlider = new QSlider(Qt::Horizontal);
    _blockSizeSlider->setRange(3, 51);  // blockSize必须是奇数
    _blockSizeSlider->setValue(_adaptiveBlockSize);

    _cSlider = new QSlider(Qt::Horizontal);
    _cSlider->setRange(-20, 20);
    _cSlider->setValue(_adaptiveC);

    // 默认隐藏滑条
    _blockSizeSliderLabel = new QLabel("BlockSize");
    _cSliderLabel = new QLabel("C");

    layout->addWidget(new QLabel("Algorithm"));
    layout->addWidget(_combo);
    layout->addWidget(new QLabel("Threshold"));
    layout->addWidget(_slider);

    layout->addWidget(_blockSizeSliderLabel);
    layout->addWidget(_blockSizeSlider);
    layout->addWidget(_cSliderLabel);
    layout->addWidget(_cSlider);

    _blockSizeSlider->hide();
    _cSlider->hide();
    _blockSizeSliderLabel->hide();
    _cSliderLabel->hide();

    connect(_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &ThresholdModel::onAlgorithmChanged);

    connect(_slider, &QSlider::valueChanged,
        this, &ThresholdModel::onThresholdChanged);
    connect(_blockSizeSlider, &QSlider::valueChanged,
        this, &ThresholdModel::onBlockSizeChanged);
    connect(_cSlider, &QSlider::valueChanged,
            this, &ThresholdModel::onCChanged);

    _algorithm = 0;
    _threshold = 128;

    return _widget;
}

// -------------------- 槽函数 --------------------
void ThresholdModel::onAlgorithmChanged(int index)
{
    _algo = static_cast<ThresholdType>(index);
    _algorithm = index;
    bool showAdaptive = (_algo == ThresholdType::AdaptiveMean ||
        _algo == ThresholdType::AdaptiveGaussian);

    _blockSizeSlider->setVisible(showAdaptive);
    _cSlider->setVisible(showAdaptive);
    _blockSizeSliderLabel->setVisible(showAdaptive);
    _cSliderLabel->setVisible(showAdaptive);
    _widget->adjustSize();

    update();
}

void ThresholdModel::onThresholdChanged(int value)
{
    _threshold = value;
    update();
}

void ThresholdModel::onBlockSizeChanged(int value)
{
    _adaptiveBlockSize = (value % 2 == 0) ? value + 1 : value;
    update();
}

void ThresholdModel::onCChanged(int value)
{
    _adaptiveC = value;
    update();
}

// -------------------- 核心处理 --------------------
void ThresholdModel::process()
{
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat _input = matData->mat();

    cv::Mat gray;
    if (_input.channels() == 4)
        cv::cvtColor(_input, gray, cv::COLOR_BGRA2GRAY);
    else if (_input.channels() == 3)
        cv::cvtColor(_input, gray, cv::COLOR_RGB2GRAY);
    else
        gray = _input;

    switch (_algorithm)
    {
        case 0: // Binary
            cv::threshold(gray, _result, _threshold, 255, cv::THRESH_BINARY);
            break;
        case 1: // BinaryInv
            cv::threshold(gray, _result, _threshold, 255, cv::THRESH_BINARY_INV);
            break;
        case 2: // Trunc
            cv::threshold(gray, _result, _threshold, 255, cv::THRESH_TRUNC);
            break;
        case 3: // ToZero
            cv::threshold(gray, _result, _threshold, 255, cv::THRESH_TOZERO);
            break;
        case 4: // AdaptiveMean
            cv::adaptiveThreshold(gray, _result, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                                  cv::THRESH_BINARY, _adaptiveBlockSize, _adaptiveC);
            break;
        case 5: // AdaptiveGaussian
            cv::adaptiveThreshold(gray, _result, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                                  cv::THRESH_BINARY, _adaptiveBlockSize, _adaptiveC);
            break;
        default:
            _result = gray.clone();
            break;
    }

    // 转回 RGB 以便 PixmapData 使用
    if (_result.channels() == 1)
        cv::cvtColor(_result, _result, cv::COLOR_GRAY2RGB);

    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<MatNodeData>(_result));
}

QJsonObject ThresholdModel::save() const
{
    QJsonObject json = BaseNodeModel::save();

    json["algorithm"] = static_cast<int>(_algo);
    json["threshold"] = _threshold;
    json["adaptiveBlockSize"] = _adaptiveBlockSize;
    json["adaptiveC"] = _adaptiveC;
    return json;
}

void ThresholdModel::load(QJsonObject const& json)
{
    if (json.contains("algorithm"))
        _algo = static_cast<ThresholdType>(json["algorithm"].toInt());
    if (json.contains("threshold"))
        _threshold = json["threshold"].toInt();
    if (json.contains("adaptiveBlockSize"))
        _adaptiveBlockSize = json["adaptiveBlockSize"].toInt();
    if (json.contains("adaptiveC"))
        _adaptiveC = json["adaptiveC"].toInt();

    // 更新滑条和显示状态
    _combo->setCurrentIndex(static_cast<int>(_algo));
    _slider->setValue(_threshold);
    _blockSizeSlider->setValue(_adaptiveBlockSize);
    _cSlider->setValue(_adaptiveC);

    // 显示/隐藏滑条
    bool showAdaptive = (_algo == ThresholdType::AdaptiveMean ||
        _algo == ThresholdType::AdaptiveGaussian);
    _blockSizeSlider->setVisible(showAdaptive);
    _cSlider->setVisible(showAdaptive);
}