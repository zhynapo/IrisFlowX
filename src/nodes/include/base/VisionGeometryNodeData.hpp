#pragma once
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>
#include <vector>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class VisionGeometryNodeData : public NodeData
{
public:

    // -------------------------
    // Types
    // -------------------------
    enum class Type
    {
        None = 0,
        Point,
        Circle,
        RotatedRect,
        Rect,
        Line,
        Contour
    };

    // -------------------------
    // NodeData interface
    // -------------------------
    NodeDataType type() const override
    {
        return NodeDataType{"visionGeometry", "VisionGeometry"};
    }

    // -------------------------
    // Public Data
    // -------------------------
    Type typeValue = Type::None;

    bool valid = false;
    cv::Point2f center;
    float angle = 0.0f;

    float radius = 0.0f;      // Circle
    cv::Size2f size;          // Rect / RotatedRect

    cv::Point2f p1, p2;       // Line
    std::vector<cv::Point> contour;  // Contour

    // -------------------------
    // Constructors
    // -------------------------
    VisionGeometryNodeData() = default;

    // -------------------------
    // Factory Methods
    // -------------------------
    static std::shared_ptr<VisionGeometryNodeData> makePoint(const cv::Point2f& pt)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::Point;
        d->valid = true;
        d->center = pt;
        return d;
    }

    static std::shared_ptr<VisionGeometryNodeData> makeCircle(const cv::Point2f& c, float r)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::Circle;
        d->valid = true;
        d->center = c;
        d->radius = r;
        return d;
    }

    static std::shared_ptr<VisionGeometryNodeData> makeRotatedRect(const cv::RotatedRect& rr)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::RotatedRect;
        d->valid = true;
        d->center = rr.center;
        d->size = rr.size;
        d->angle = rr.angle;
        return d;
    }

    static std::shared_ptr<VisionGeometryNodeData> makeRect(const cv::Rect& rc)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::Rect;
        d->valid = true;
        d->center = cv::Point2f(rc.x + rc.width*0.5f, rc.y + rc.height*0.5f);
        d->size = cv::Size2f((float)rc.width, (float)rc.height);
        d->angle = 0.f;
        return d;
    }

    static std::shared_ptr<VisionGeometryNodeData> makeLine(const cv::Point2f& a, const cv::Point2f& b)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::Line;
        d->valid = true;
        d->p1 = a;
        d->p2 = b;
        return d;
    }

    static std::shared_ptr<VisionGeometryNodeData> makeContour(const std::vector<cv::Point>& c)
    {
        auto d = std::make_shared<VisionGeometryNodeData>();
        d->typeValue = Type::Contour;
        d->valid = true;
        d->contour = c;
        return d;
    }
};