#include "Blend.hpp"
#include "Nodetype.hpp"
#include "utils.hpp"

const NodeDesc Blend::desc =
{
    "Blend",
    "Blend",
    "Image",

    // inputs
    {
        PortDesc::In("points", NodeType::Mat),
        PortDesc::In("points", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("out", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeDouble("alpha", "alpha", 0.5),
        ParamDesc::makeBool("closed", "closed", true)
    }
};

Blend::Blend()
    : BaseNodeModel(desc)
{
}


void Blend::process()
{
    auto mat_A = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!mat_A)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat A = mat_A->mat();

    auto mat_B = std::dynamic_pointer_cast<MatNodeData>(_getInput(1));
    if (!mat_B)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat B = mat_B->mat();

    // TODO: implement algorithm
    matchTypeSizeChannel(A, B);
    if (A.size() != B.size() || A.type() != B.type())
    {
        setOutputData(0, nullptr);
        return;
    }



    float alpha = parameterValue("alpha").toDouble();
    float beta = parameterValue("beta").toDouble();
    // TODO: implement algorithm
    cv::Mat _output = A * alpha + B * beta;

    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
