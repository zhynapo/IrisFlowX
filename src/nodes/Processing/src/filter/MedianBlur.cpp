#include "MedianBlur.hpp"

const NodeDesc MedianBlur::desc =
{
    "MedianBlur",
    "Median Blur",
    CAT_PROC_FILTER,

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeInt("ksize", "ksize", 3).range(1, 100).setStep(2)
    }
};

MedianBlur::MedianBlur()
    : BaseNodeModel(desc)
{
}

void MedianBlur::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();
    int k = parameterValue("ksize").value<int>();
    cv::medianBlur(_input, _output, k);

    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
