#include "WarpAffine.hpp"
#include <opencv2/imgproc.hpp>

const NodeDesc WarpAffine::desc = {
    "WarpAffine",
    "Affine",
    CAT_PROC_GEOMETRY,
    {
        PortDesc::In("mat", NodeType::Mat),
        PortDesc::In("matrix", NodeType::AffineMatrix)
    },
    {
        PortDesc::Out("mat", NodeType::Mat)
    },
    {}
};

WarpAffine::WarpAffine() : BaseNodeModel(desc)
{
}

void WarpAffine::process()
{
    auto imgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    auto Mdata   = std::dynamic_pointer_cast<AffineMatrixData>(_getInput(1));

    if (!imgData || !Mdata ) {
        _out = cv::Mat();
        return;
    }

    cv::Mat img = imgData->mat();
    cv::Mat M   = Mdata->mat;
    if (M.empty())
    {
        _out = cv::Mat();
        return;
    }
    cv::warpAffine(img, _out, M, img.size());
}

std::shared_ptr<NodeData> WarpAffine::outData(PortIndex)
{
    if (_out.empty()) return nullptr;
    return std::make_shared<MatNodeData>(_out);
}