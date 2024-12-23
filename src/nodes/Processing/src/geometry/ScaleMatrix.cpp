#include "ScaleMatrix.hpp"

using namespace QtNodes;
using namespace Flow;

const NodeDesc ScaleMatrix::desc = {
    "ScaleMatrix",
    "Affine",
    CAT_PROC_GEOMETRY,
    {},
    { PortDesc::Out("matrix", NodeType::AffineMatrix) },
    {
        ParamDesc::makeDouble("sx", "Scale X", 1.0),
        ParamDesc::makeDouble("sy", "Scale Y", 1.0)
    }
};

ScaleMatrix::ScaleMatrix() : BaseNodeModel(desc)
{
}

void ScaleMatrix::process()
{
    double sx = parameterValue("sx").toDouble();
    double sy = parameterValue("sy").toDouble();

    _M = (cv::Mat_<double>(2,3) << sx,0,0, 0,sy,0);
}

std::shared_ptr<NodeData> ScaleMatrix::outData(PortIndex)
{
    return std::make_shared<AffineMatrixData>(_M);
}