#include "AddWeighted.hpp"

const NodeDesc AddWeighted::desc =
{
    "AddWeighted",
    "AddWeighted",
    CAT_PROC_BASIC,
    
    // inputs
    {
        PortDesc::In("mat", NodeType::Mat),
        PortDesc::In("mat", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeDouble("Alpha", "Alpha", 1.0),
        ParamDesc::makeDouble("Beta", "Beta", 1.0),
        ParamDesc::makeDouble("Gamma", "Gamma", 1.0)
    }
};

//REGISTER_NODE(AddWeighted)

AddWeighted::AddWeighted()
    : BaseNodeModel(desc)
{
}


void AddWeighted::process()
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

    // TODO: implement algorithm
    cv::Mat out;
    float alpha = parameterValue("Alpha").toDouble();
    float beta = parameterValue("Beta").toDouble();
    float gamma = parameterValue("Gamma").toDouble();
    cv::addWeighted(A, alpha, B, beta, gamma, out);

    setOutputData(0, std::make_shared<MatNodeData>(out));
}
