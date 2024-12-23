#include "TranslateMatrix.hpp"

using namespace QtNodes;
using namespace Flow;

const NodeDesc TranslateMatrix::desc = {
    "TranslateMatrix",
    "Affine",
    CAT_PROC_GEOMETRY,
    {},
    { PortDesc::Out("matrix", NodeType::AffineMatrix) },
    {
        ParamDesc::makeDouble("dx", "Move X", 0.0),
        ParamDesc::makeDouble("dy", "Move Y", 0.0)
    }
};

TranslateMatrix::TranslateMatrix() : BaseNodeModel(desc)
{
    _M = cv::Mat::eye(2, 3, CV_64F);
}

void TranslateMatrix::process()
{
    double dx = parameterValue("dx").toDouble();
    double dy = parameterValue("dy").toDouble();

    _M = (cv::Mat_<double>(2,3) << 1,0,dx, 0,1,dy);
}

std::shared_ptr<NodeData> TranslateMatrix::outData(PortIndex)
{
    return std::make_shared<AffineMatrixData>(_M);
}