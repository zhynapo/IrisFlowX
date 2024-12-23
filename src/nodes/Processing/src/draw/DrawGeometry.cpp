#include "DrawGeometry.hpp"

const NodeDesc DrawGeometry::desc =
{
    "DrawGeometry",
    "DrawGeometry",
    CAT_PROC_DRAW,

    // inputs
    {
        PortDesc::In("mat", NodeType::Mat),
        PortDesc::In("geometry", NodeType::VisionGeometry)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeColor("color", "Color", QColor(0,255,0)),
        ParamDesc::makeInt("thickness", "Thickness", 2).range(1, 10),
        ParamDesc::makeInt("crossSize", "CrossSize", 10).range(1, 50)
    }
};

DrawGeometry::DrawGeometry()
    : BaseNodeModel(desc)
{
}

void DrawGeometry::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    auto geoData = std::dynamic_pointer_cast<VisionGeometryNodeData>(_getInput(1));

    if (!matData || !geoData)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat output = matData->mat().clone();

    QColor color = parameterValue("color").value<QColor>();
    int thickness = parameterValue("thickness").toInt();
    int crossSize = parameterValue("crossSize").toInt();

    cv::Scalar cvColor(color.blue(), color.green(), color.red());

    switch (geoData->typeValue)
    {
    case VisionGeometryNodeData::Type::Point:
    {
        cv::circle(output, geoData->center, 5, cvColor, -1);
        break;
    }

    case VisionGeometryNodeData::Type::Circle:
    {
        cv::circle(output, geoData->center, geoData->radius, cvColor, thickness);
        break;
    }

    case VisionGeometryNodeData::Type::RotatedRect:
    {
        cv::RotatedRect rr(geoData->center, geoData->size, geoData->angle);
        cv::Point2f pts[4];
        rr.points(pts);

        for (int i = 0; i < 4; ++i)
            cv::line(output, pts[i], pts[(i + 1) % 4], cvColor, thickness);
        break;
    }

    case VisionGeometryNodeData::Type::Contour:
    {
        std::vector<std::vector<cv::Point>> cs;
        cs.push_back(geoData->contour);
        cv::drawContours(output, cs, -1, cvColor, thickness);
        break;
    }

    default:
        break;
    }

    // 画十字中心
    cv::Point2f c = geoData->center;
    cv::line(output, { int(c.x - crossSize), int(c.y) }, { int(c.x + crossSize), int(c.y) }, cvColor, thickness);
    cv::line(output, { int(c.x), int(c.y - crossSize) }, { int(c.x), int(c.y + crossSize) }, cvColor, thickness);

    setOutputData(0, std::make_shared<MatNodeData>(output));
}
