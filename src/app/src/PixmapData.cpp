#include "PixmapData.hpp"

PixmapData::PixmapData() {}
PixmapData::PixmapData(QPixmap const& pixmap) : _pixmap(pixmap) {}

QtNodes::NodeDataType PixmapData::type() const
{
    return { "pixmap", "QPixmap" };
}

QPixmap const& PixmapData::pixmap() const
{
    return _pixmap;
}