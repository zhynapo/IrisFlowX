#include "CombineAffine.hpp"

using namespace QtNodes;
using namespace Flow;

const NodeDesc CombineAffine::desc = {
    "CombineAffine",
    "Affine",
    CAT_PROC_GEOMETRY,
    {
        PortDesc::In("A", NodeType::AffineMatrix),
        PortDesc::In("B", NodeType::AffineMatrix)
    },
    {
        PortDesc::Out("matrix", NodeType::AffineMatrix)
    },
    {}
};

CombineAffine::CombineAffine() : BaseNodeModel(desc)
{
}

void CombineAffine::process()
{
    auto A = std::dynamic_pointer_cast<AffineMatrixData>(_getInput(0));
    auto B = std::dynamic_pointer_cast<AffineMatrixData>(_getInput(1));

    if (!A || !B) {
        _M = cv::Mat::eye(2,3,CV_64F);
        return;
    }

    cv::Mat A3 = cv::Mat::eye(3,3,CV_64F);
    cv::Mat B3 = cv::Mat::eye(3,3,CV_64F);

    A->mat.copyTo(A3(cv::Rect(0,0,3,2)));
    B->mat.copyTo(B3(cv::Rect(0,0,3,2)));

    cv::Mat C3 = A3 * B3;

    _M = C3(cv::Rect(0,0,3,2)).clone();
}

std::shared_ptr<NodeData> CombineAffine::outData(PortIndex)
{
    return std::make_shared<AffineMatrixData>(_M);
}