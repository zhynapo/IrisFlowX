
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
#include "Gaussianblur.hpp"
#include "Nodetype.hpp"

const NodeDesc GaussianBlur::desc =
{
    "GaussianBlur",
    "GaussianBlur",
    CAT_PROC_FILTER_SMOOTH,
    { PortDesc::In("mat", NodeType::Mat) },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        ParamDesc::makeInt("kernelSize", "kernelSize", 3).setRange(1, 100).setStep(2),
        ParamDesc::makeDouble("sigma", "sigma", 1.0),
        ParamDesc::makeBool("normalize", "normalize", false),
    }
};

// --------------- 自动注册 ----------------
//REGISTER_NODE(GaussianBlur)

GaussianBlur::GaussianBlur()
    :BaseNodeModel(GaussianBlur::desc)
{
 
}

void GaussianBlur::process()
{
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
    int k = parameterValue("kernelSize").value<int>();

    if (k < 3) k = 3;
    if ((k & 1) == 0) k += 1;
    double sigma = parameterValue("sigma").value<int>();

    // sigma=0 由 OpenCV 根据 k 自动推导
    cv::Mat output;
    cv::GaussianBlur(inputMat, output, cv::Size(k, k), sigma, 0.0, cv::BORDER_REPLICATE);

    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<MatNodeData>(output));
}
