#include "ConvertcolorNode.hpp"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonValue>
#include <opencv2/imgproc.hpp>

static QVector<QString> COLOR_NAMES = []() {
    QVector<QString> n;
    for (int i = 0; i < COLOR_OPTIONS_COUNT; i++)
        n.push_back(COLOR_OPTIONS[i].name);
    return n;
    }();

static QVector<int> COLOR_VALUES = []() {
    QVector<int> v;
    for (int i = 0; i < COLOR_OPTIONS_COUNT; i++)
        v.push_back(COLOR_OPTIONS[i].option);
    return v;
    }();

const NodeDesc ConvertcolorNode::desc = {
    "Convert Color",
    "ConvertColorNodeModel",
    CAT_PROC_BASIC,
    {
        PortDesc::In("image", NodeType::Mat)
    },
    {
        PortDesc::Out("output", NodeType::Mat)
    },

    {
        ParamDesc::makeCombo(
            "convert_mode",
            "Conversion Mode",
            COLOR_NAMES,
            COLOR_VALUES,
            COLOR_OPTIONS[0].option
        )
    }
};


ConvertcolorNode::ConvertcolorNode()
    :BaseNodeModel(desc)
{

}

void ConvertcolorNode::process()
{
    // TODO: put your OpenCV processing logic here.
    // Default: passthrough
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat inputMat = matData->mat();

    //----- 4. 获取参数，例如颜色 -----
    cv::Mat output;

    int mode = parameterValue("convert_mode").toInt();

    cv::cvtColor(inputMat, output, mode);

    setOutputData(0, std::make_shared<MatNodeData>(output));

    qDebug() << "Convert mode:" << mode
         << " output type:" << output.type()
         << " channels:" << output.channels();
}
