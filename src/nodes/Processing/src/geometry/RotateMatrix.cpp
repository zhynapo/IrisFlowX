#include "RotateMatrix.hpp"

using namespace QtNodes;
using namespace Flow;

const NodeDesc RotateMatrix::desc = {
    "RotateMatrix",
    "Affine",
    CAT_PROC_GEOMETRY,
    {},
    { PortDesc::Out("matrix", NodeType::AffineMatrix) },
    {
        ParamDesc::makeDouble("angle", "Angle (deg)", 0.0),
        ParamDesc::makeDouble("scale", "Scale", 1.0),
        ParamDesc::makeDouble("cx", "Center X", 0.0),
        ParamDesc::makeDouble("cy", "Center Y", 0.0)
    }
};

RotateMatrix::RotateMatrix() : BaseNodeModel(desc)
{
    _M = cv::getRotationMatrix2D(cv::Point2f(0, 0), 0, 1);
}

void RotateMatrix::process()
{
    double angle = parameterValue("angle").toDouble();
    double scale = parameterValue("scale").toDouble();
    double cx = parameterValue("cx").toDouble();
    double cy = parameterValue("cy").toDouble();

    _M = cv::getRotationMatrix2D(cv::Point2f(cx,cy), angle, scale);
    std::cout << _M;
}

std::shared_ptr<NodeData> RotateMatrix::outData(PortIndex)
{
    return std::make_shared<AffineMatrixData>(_M);
}