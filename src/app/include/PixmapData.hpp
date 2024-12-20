#pragma once
#include <QtNodes/NodeData>
#include <QPixmap>

class PixmapData : public QtNodes::NodeData
{
public:
    PixmapData();
    explicit PixmapData(QPixmap const& pixmap);

    QtNodes::NodeDataType type() const override;
    QPixmap const& pixmap() const;

private:
    QPixmap _pixmap;
};

