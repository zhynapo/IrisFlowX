#include "MinAreaRect.hpp"
#include "base/ContoursNodeData.hpp"

const NodeDesc MinAreaRect::desc =
{
    "MinAreaRect",
    "MinAreaRect (4 Corner Points)",
    CAT_ANALY_CONTOURS,

    // inputs
    {
        PortDesc::In("contours", NodeType::Contours)
    },

    // outputs
    {
        PortDesc::Out("rotated_rect", NodeType::VisionGeometry)
    },

    // parameters
    {
        ParamDesc::makeDouble("epsilon", "epsilon", 1.0),
        ParamDesc::makeBool("closed", "closed", true)
    }
};

MinAreaRect::MinAreaRect()
    : BaseNodeModel(desc)
{
}

void MinAreaRect::process()
{
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    if (!contourData)
    {
        setOutputData(0, nullptr);
        return;
    }
    const std::vector<std::vector<cv::Point>>& contours = contourData->value();

    if (contours.empty())
    {
        setOutputData(0, nullptr);
        return;
    }
    
    // Process the first contour in the list
    const std::vector<cv::Point>& contour = contours[0];
    
    if (contour.size() < 2)
    {
        setOutputData(0, nullptr);
        return;
    }
    
    auto geom = std::make_shared<VisionGeometryNodeData>();

    geom->typeValue = VisionGeometryNodeData::Type::Contour;
    geom->valid = false;   // 先设为 false
    cv::RotatedRect rr = cv::minAreaRect(contour);
    
    // 获取旋转矩形的4个角点
    cv::Point2f vertices[4];
    rr.points(vertices);
    
    // 将4个角点转换为整数坐标并存储到contour中
    geom->contour.clear();
    for (int i = 0; i < 4; ++i) {
        geom->contour.push_back(cv::Point(static_cast<int>(vertices[i].x), static_cast<int>(vertices[i].y)));
    }
    
    geom->center = rr.center;
    geom->valid = true;    // 计算成功，设为 true

    setOutputData(0, geom);
}