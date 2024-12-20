#include "widgets/CurveCanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>
#include <QPainterPath>

CurveCanvas::CurveCanvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(200, 200);
    _pts = { {0,0}, {1,1} };
}

void CurveCanvas::setPoints(const std::vector<Pt>& p)
{
    _pts = p;
    update();
}

QPointF CurveCanvas::toCanvas(const Pt& p) const
{
    return {
        p.x * width(),
        (1.0f - p.y) * height()
    };
}

Pt CurveCanvas::toPoint(const QPointF& p) const
{
    return {
        float(std::clamp(p.x() / width(),  0.0, 1.0)),
        float(std::clamp(1.0 - p.y() / height(), 0.0, 1.0))
    };
}

int CurveCanvas::pickPoint(const QPointF& pos) const
{
    for (int i = 0; i < _pts.size(); ++i)
    {
        QPointF c = toCanvas(_pts[i]);
        if (QLineF(c, pos).length() < _pickRadius * width())
            return i;
    }
    return -1;
}

void CurveCanvas::mousePressEvent(QMouseEvent* e)
{
    QPointF pos = e->pos();
    _activePoint = pickPoint(pos);

    if (_activePoint < 0 && e->button() == Qt::LeftButton)
    {
        // 新增点
        Pt p = toPoint(pos);
        _pts.push_back(p);
        std::sort(_pts.begin(), _pts.end(),
            [](auto& a, auto& b) { return a.x < b.x; });
        _activePoint = pickPoint(pos);
        emit curveChanged();
        update();
    }
}

void CurveCanvas::mouseMoveEvent(QMouseEvent* e)
{
    if (_activePoint < 0) return;

    Pt p = toPoint(e->pos());

    // 锁 x 顺序（首尾不动）
    if (_activePoint == 0)
        p.x = 0;
    else if (_activePoint == _pts.size() - 1)
        p.x = 1;
    else
    {
        p.x = std::clamp(
            p.x,
            _pts[_activePoint - 1].x + 0.01f,
            _pts[_activePoint + 1].x - 0.01f
        );
    }

    _pts[_activePoint] = p;
    emit curveChanged();
    update();
}

void CurveCanvas::mouseReleaseEvent(QMouseEvent*)
{
    _activePoint = -1;
}

float CurveCanvas::catmull(float p0, float p1, float p2, float p3, float t) const
{
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * (
        2 * p1 +
        (-p0 + p2) * t +
        (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 +
        (-p0 + 3 * p1 - 3 * p2 + p3) * t3
        );
}

void CurveCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(30, 30, 30));

    // grid
    p.setPen(QColor(60, 60, 60));
    for (int i = 1; i < 4; ++i)
    {
        p.drawLine(i * width() / 4, 0, i * width() / 4, height());
        p.drawLine(0, i * height() / 4, width(), i * height() / 4);
    }

    if (_pts.size() < 2) return;

    // curve
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::green, 2));

    QPainterPath path;
    path.moveTo(toCanvas(_pts[0]));

    for (int i = 0; i + 1 < _pts.size(); ++i)
    {
        int i0 = std::max(0, i - 1);
        int i1 = i;
        int i2 = i + 1;
        int i3 = std::min(i + 2, int(_pts.size() - 1));

        for (int s = 1; s <= 16; ++s)
        {
            float t = s / 16.0f;
            float x = catmull(
                _pts[i0].x, _pts[i1].x,
                _pts[i2].x, _pts[i3].x, t
            );
            float y = catmull(
                _pts[i0].y, _pts[i1].y,
                _pts[i2].y, _pts[i3].y, t
            );
            path.lineTo(toCanvas({ x,y }));
        }
    }
    p.drawPath(path);

    // points
    p.setBrush(Qt::white);
    for (auto& pt : _pts)
    {
        QPointF c = toCanvas(pt);
        p.drawEllipse(c, 4, 4);
    }
}
