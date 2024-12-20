#include "LoadImage.hpp"
#include "Nodetype.hpp"

const NodeDesc LoadImage::desc =
{
    "LoadImage",
    "Load Image",
    "IO",

    // inputs
    {
        
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeString("path", "path", QString(""))
    }
};

LoadImage::LoadImage()
    : BaseNodeModel(desc)
{
}

void LoadImage::process()
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
