#include "Threshold.hpp"
#include "Nodetype.hpp"

const NodeDesc Threshold::desc =
{
    "Threshold",
    "Threshold",
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
        ParamDesc::makeInt("thresh", "thresh", 128).setRange(0, 255),
        ParamDesc::makeInt("maxval", "maxval", 255).setRange(0, 255),
        ParamDesc::makeCombo("type", "type", QVector<QString>{ QString("Binary"), QString("BinaryInv"), QString("Trunc"), QString("ToZero"), QString("ToZeroInv") }, QVector<int>{ 0, 1, 2, 3, 4 }, 0)
    }
};

Threshold::Threshold()
    : BaseNodeModel(desc)
{
}

void Threshold::process()
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
