#include "Test.hpp"

const NodeDesc Test::desc =
{
    "Test",
    "Test",
    "Processing/Filter",

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
        ParamDesc::makeInt("thresh", "thresh", 128).range(0, 255),
        ParamDesc::makeInt("maxval", "maxval", 255).range(0, 255),
        ParamDesc::makeCombo("type", "type", QVector<QString>{ QString("Binary"), QString("BinaryInv"), QString("Trunc"), QString("ToZero"), QString("ToZeroInv") }, QVector<int>{ 0, 1, 2, 3, 4 }, 0)
    }
};

Test::Test()
    : BaseNodeModel(desc)
{
}

void Test::process()
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
