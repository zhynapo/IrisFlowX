
#include "ResizeNode.hpp"


const NodeDesc ResizeNode::desc =
{
    "Resize",
    "Resize",
    CAT_PROC_GEOMETRY,
    { PortDesc::In("mat", NodeType::Mat) },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        ParamDesc::makeDouble("scaleX", "Scale X", 1.0)
            .setRange(0.01, 100.0)
            .setStep(0.01),

        ParamDesc::makeDouble("scaleY", "Scale Y", 1.0)
            .setRange(0.01, 100.0)
            .setStep(0.01),

        ParamDesc::makeCombo(
            "interpolation",
            "Interpolation",
            {"Nearest", "Linear", "Cubic", "Area", "Lanczos4"},
            {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4},
            cv::INTER_LINEAR
        )
    }
};

ResizeNode::ResizeNode()
    :BaseNodeModel(desc)   
{
}

#if 0
QVector<ParamDesc> ResizeNode::parameters() const
{
    return {
        ParamDesc::makeDouble("scaleX", "Scale X", 1.0)
            .setRange(0.01, 100.0)
            .setStep(0.01),

        ParamDesc::makeDouble("scaleY", "Scale Y", 1.0)
            .setRange(0.01, 100.0)
            .setStep(0.01),

        ParamDesc::makeCombo(
            "interpolation",
            "Interpolation",
            {"Nearest", "Linear", "Cubic", "Area", "Lanczos4"},
            {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4},
            cv::INTER_LINEAR
        )
    };
}
#endif

void ResizeNode::process()
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
    double scaleX = parameterValue("scaleX").value<double>();
    double scaleY = parameterValue("scaleY").value<double>();
    int interpolation = parameterValue("interpolation").value<int>();
    
    cv::Size size = inputMat.size();
    double minScale = 1.0 / std::max(size.width, size.height);

    double fx = std::max(scaleX, minScale);
    double fy = std::max(scaleY, minScale);

    cv::resize(inputMat, _output, cv::Size(), fx, fy, interpolation);
    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
