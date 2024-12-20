#pragma once
#include <QtNodes/NodeDelegateModel>
#include "base/MatNodeData.hpp"
#include "base/PointNodeData.hpp"
#include "base/PointsNodeData.hpp"
#include "base/NumberNodeData.hpp"
#include "base/RotatedRectNodeData.hpp"

using QtNodes::NodeDataType;

namespace NodeType
{
    static inline const NodeDataType Mat      = {"mat",    "Mat"};
    static inline const NodeDataType Points = { "points", "Points" };
    static inline const NodeDataType Point = { "point", "Point" };
    static inline const NodeDataType Image    = {"image",  "Image"};
    static inline const NodeDataType Float    = {"float",  "Float"};
    static inline const NodeDataType Integer  = {"int",    "Int"};

    static inline const NodeDataType Boolean = { "bool", "Boolean" };
    static inline const NodeDataType Contours = { "contours", "Contours" };
    static inline const NodeDataType RotatedRect = { "rotatedrect", "RotatedRect" };

}

