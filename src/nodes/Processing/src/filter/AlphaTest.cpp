#include "AlphaTest.hpp"

const NodeDesc AlphaTest::desc =
{
    "AlphaTest",
    "AlphaTest",
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
        ParamDesc::makeDouble("alpha", "alpha", 0.5).setRange(0.0, 1.0)
    }
};

AlphaTest::AlphaTest()
    : BaseNodeModel(desc)
{
}

void AlphaTest::process()
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
