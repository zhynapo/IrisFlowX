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
        ParamDesc::makeCombo("drawMode", "Draw Mode", 
            QVector<QString>{"Full Shape", "Corner Points", "Center Cross"},
            QVector<int>{0, 1, 2},
            0),
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

    int drawMode = parameterValue("drawMode").toInt();
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
        if (drawMode == 0) { // Full Shape
            cv::circle(output, geoData->center, geoData->radius, cvColor, thickness);
        } else if (drawMode == 1) { // Corner Points - Not applicable for Circle
            cv::circle(output, geoData->center, geoData->radius, cvColor, thickness);
        }
        break;
    }

    case VisionGeometryNodeData::Type::RotatedRect:
    {
        cv::RotatedRect rr(geoData->center, geoData->size, geoData->angle);
        cv::Point2f pts[4];
        rr.points(pts);

        if (drawMode == 0) { // Full Shape - Draw the full rectangle
            for (int i = 0; i < 4; ++i)
                cv::line(output, pts[i], pts[(i + 1) % 4], cvColor, thickness);
        } else if (drawMode == 1) { // Corner Points - Draw only the 4 corner points
            for (int i = 0; i < 4; ++i) {
                cv::circle(output, pts[i], thickness + 2, cvColor, 3); // Filled circles at corners

                cv::Point2f c = pts[i];
                cv::line(output, { int(c.x - crossSize), int(c.y) }, { int(c.x + crossSize), int(c.y) }, cvColor, thickness);
                cv::line(output, { int(c.x), int(c.y - crossSize) }, { int(c.x), int(c.y + crossSize) }, cvColor, thickness);
            }
        }
        break;
    }

    case VisionGeometryNodeData::Type::Contour:
    {
        if (drawMode == 0) { // Full Shape
            std::vector<std::vector<cv::Point>> cs;
            cs.push_back(geoData->contour);
            cv::drawContours(output, cs, -1, cvColor, thickness);
        } else if (drawMode == 1) { // Corner Points - Treat each point in contour as a corner
            for (const auto& point : geoData->contour) {
                cv::circle(output, point, thickness + 2, cvColor, -1); // Filled circles at points
            }
        }
        break;
    }

    default:
        break;
    }

    // 画十字中心 - only if not in corner points mode
    if (drawMode != 1) {
        cv::Point2f c = geoData->center;
        cv::line(output, { int(c.x - crossSize), int(c.y) }, { int(c.x + crossSize), int(c.y) }, cvColor, thickness);
        cv::line(output, { int(c.x), int(c.y - crossSize) }, { int(c.x), int(c.y + crossSize) }, cvColor, thickness);
    }

    setOutputData(0, std::make_shared<MatNodeData>(output));
}