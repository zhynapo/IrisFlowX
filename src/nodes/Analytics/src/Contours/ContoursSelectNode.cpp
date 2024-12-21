
#include "ContoursSelectNode.hpp"

const NodeDesc ContoursSelectNode::desc =
{
    "ContoursSelect",
    "Contours Select",
    CAT_ANALY_CONTOURS,                  // category
    // ports
    { PortDesc::In("Contours", NodeType::Contours) },
    { PortDesc::Out("Points", NodeType::Points) },

    // ----------- params -------------
    {
        ParamDesc::makeCombo(
            "mode",
            "Mode",
            { "By Index", "Largest Area" },     // comboNames
            { 0, 1 },                           // comboValues（你可以自定义）
            0                                   // default value index (0 = By Index)
        ),

        ParamDesc::makeInt(
            "index",
            "Index",
            0).setRange(0, 9999)
    }
};



ContoursSelectNode::ContoursSelectNode()
    :BaseNodeModel(desc)
{

}

void ContoursSelectNode::process()
{
    //----- 1. 获取Contours输入 -----
    auto contourtData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    if (!contourtData)
    {
        setOutputData(0, nullptr);
        return;
    }


    std::vector<std::vector<cv::Point>> _contours = contourtData->value();
    if (_contours.empty()) return;
    int n = _contours.size();
    setParamRange("index", 0, _contours.size() - 1);
    setParamEnabled("index", n > 0);

    //----- 4. 获取参数，例如颜色 -----
    int k = parameterValue("mode").value<int>();

    if (k == 0) {
        int idx = parameterValue("index").value<int>();
        if (idx >= 0 && idx < (int)_contours.size()) {
            _selectedContour = _contours[idx];
        }
    }
    else if (k == 1) {
        double maxArea = 0;
        int bestIdx = -1;
        for (int i = 0; i < (int)_contours.size(); i++) {
            double a = cv::contourArea(_contours[i]);
            if (a > maxArea) {
                maxArea = a;
                bestIdx = i;
            }
        }
        if (bestIdx >= 0)
            _selectedContour = _contours[bestIdx];
    }
    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<PointsNodeData>(_selectedContour));
}

