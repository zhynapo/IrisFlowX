#include "DrawContours.hpp"
#include "base/ContoursNodeData.hpp"

const NodeDesc DrawContours::desc =
{
    "Draw Contours",
    "DrawContours",
    CAT_PROC_DRAW,

    // inputs
    {
        PortDesc::In("image", NodeType::Mat),
        PortDesc::In("contours", NodeType::Contours)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat)
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

DrawContours::DrawContours()
    : BaseNodeModel(desc)
{
}

void DrawContours::process()
{
    auto imgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(1));

    if (!imgData || !contourData)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat output = imgData->mat().clone();

    int drawMode = parameterValue("drawMode").toInt();
    QColor color = parameterValue("color").value<QColor>();
    int thickness = parameterValue("thickness").toInt();
    int crossSize = parameterValue("crossSize").toInt();

    cv::Scalar cvColor(color.blue(), color.green(), color.red());

    auto contours = contourData->value();
    
    if (drawMode == 0) { // Full Shape - Draw all contours
        cv::drawContours(output, contours, -1, cvColor, thickness);
    } else if (drawMode == 1) { // Corner Points - Draw each point in each contour
        for (const auto& contour : contours) {
            for (const auto& point : contour) {
                cv::circle(output, point, thickness + 2, cvColor, -1); // Filled circles at points
            }
        }
    } else if (drawMode == 2) { // Center Cross - Draw center cross for each point in each contour
        for (const auto& contour : contours) {
            for (const auto& point : contour) {
                cv::line(output, 
                    { point.x - crossSize, point.y }, 
                    { point.x + crossSize, point.y }, 
                    cvColor, thickness);
                cv::line(output, 
                    { point.x, point.y - crossSize }, 
                    { point.x, point.y + crossSize }, 
                    cvColor, thickness);
            }
        }
    }

    setOutputData(0, std::make_shared<MatNodeData>(output));
}