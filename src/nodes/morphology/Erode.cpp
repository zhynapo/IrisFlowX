#include "Erode.hpp"
#include "Nodetype.hpp"

const NodeDesc Erode::desc =
{
    "Erode",
    "Erode",
    "Morphology",

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
        ParamDesc::makeInt("iterations", "iterations", 1).setRange(1, 10)
    }
};

Erode::Erode()
    : BaseNodeModel(desc)
{
}

void Erode::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    // TODO: implement algorithm
    _output = _input.clone();

    setOutputData(0, std::make_shared<MatNodeData>(_output));
}
