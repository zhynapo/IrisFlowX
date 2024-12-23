#include "ShearMatrix.hpp"

using namespace QtNodes;
using namespace Flow;

const NodeDesc ShearMatrix::desc = {
    "ShearMatrix",
    "Affine",
    CAT_PROC_GEOMETRY,
    {},
    { PortDesc::Out("matrix", NodeType::AffineMatrix) },
    {
        ParamDesc::makeDouble("kx", "Shear X", 0.0),
        ParamDesc::makeDouble("ky", "Shear Y", 0.0)
    }
};

ShearMatrix::ShearMatrix() : BaseNodeModel(desc)
{
}

void ShearMatrix::process()
{
    double kx = parameterValue("kx").toDouble();
    double ky = parameterValue("ky").toDouble();

    _M = (cv::Mat_<double>(2,3) << 1, kx, 0, ky, 1, 0);
    std::cout<<_M;
}

std::shared_ptr<NodeData> ShearMatrix::outData(PortIndex)
{
    return std::make_shared<AffineMatrixData>(_M);
}