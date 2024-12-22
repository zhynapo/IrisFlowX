#include "BilateralFilter.hpp"

const NodeDesc BilateralFilter::desc =
{
    "BilateralFilter",
    "Bilateral Filter",
    CAT_PROC_FILTER_SMOOTH,

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
        ParamDesc::makeInt("ksize", "ksize", 3).setRange(1, 31).setStep(2),
        ParamDesc::makeDouble("sigmaColor", "sigmaColor", 75.0),
        ParamDesc::makeDouble("sigmaSpace", "sigmaSpace", 75.0)
    }
};

BilateralFilter::BilateralFilter()
    : BaseNodeModel(desc)
{
}

void BilateralFilter::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    // TODO: implement algorithm
    int k = parameterValue("ksize").value<int>();
    double sigmaColor = parameterValue("sigmaColor").value<double>();
    double sigmaSpace = parameterValue("sigmaSpace").value<double>();
    cv::bilateralFilter(_input, _output, k, sigmaColor, sigmaSpace);

    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
