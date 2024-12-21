
#include "FindContoursNode.hpp"

const NodeDesc FindContoursNode::desc =
{
    "FindContours",
    "FindContours",
    CAT_ANALY_CONTOURS,
    { PortDesc::In("mat", NodeType::Mat) },
    { PortDesc::Out("Contours", NodeType::Contours) },
    {
    }
};

FindContoursNode::FindContoursNode()
    :BaseNodeModel(FindContoursNode::desc)
{
 
}

void FindContoursNode::process() 
{
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat inputMat = matData->mat();

    cv::Mat gray;
    if(inputMat.channels() > 1)
        cv::cvtColor(inputMat, gray, cv::COLOR_BGR2GRAY);
    else
        gray = inputMat;

    cv::Mat bin;
    if(cv::countNonZero(gray) != gray.total())  // 非全黑图像
        cv::threshold(gray, bin, 127, 255, cv::THRESH_BINARY);
    else
        bin = gray;

    _contours.clear();
    //cv::findContours(bin, _contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(bin, _contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<ContoursNodeData>(_contours));
}
