
#include "ErodeNode.hpp"


const NodeDesc ErodeNode::desc =
{
    "ErodeNode",
    "ErodeNode",
    CAT_PROC_MORPH,
    { PortDesc::In("mat", NodeType::Mat) },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        // kernel size (must be odd)
        ParamDesc::makeInt("kernelSize", "Kernel Size", 3)
            .range(1, 99)
            .setStep(2),     // step = 2 → always odd

        // iterations
        ParamDesc::makeInt("iterations", "Iterations", 1)
            .range(1, 20)
            .setStep(1),

        // border type
        ParamDesc::makeCombo(
            "borderType",
            "Border Type",
            {"Constant", "Replicate", "Reflect", "Reflect101", "Isolated"},
            {cv::BORDER_CONSTANT,
             cv::BORDER_REPLICATE,
             cv::BORDER_REFLECT,
             cv::BORDER_REFLECT_101,
             cv::BORDER_ISOLATED},
            cv::BORDER_CONSTANT
        )
    }
};

ErodeNode::ErodeNode()
    :BaseNodeModel(ErodeNode::desc)
{
}

void ErodeNode::process()
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
    int _kernelSize = parameterValue("kernelSize").value<int>();
    int _iterations = parameterValue("iterations").value<int>();
    int _borderType = parameterValue("borderType").value<int>();
    // Ensure kernel size is always odd
    int k = std::max(1, _kernelSize);
    if (k % 2 == 0) k += 1;

    cv::Mat kernel = cv::getStructuringElement(
        cv::MORPH_RECT,
        cv::Size(k, k)
    );

    try {
        cv::erode(inputMat, _output, kernel,
                  cv::Point(-1, -1),
                  _iterations,
                  _borderType);
    }
    catch (cv::Exception& e)
    {
        qDebug("cv::erode failed: %s", e.what());
        _output = _input.clone();   // fail-safe
    }
    
    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
