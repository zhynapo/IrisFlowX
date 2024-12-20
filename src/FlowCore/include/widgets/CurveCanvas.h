#pragma once
#include <QWidget>
#include <vector>

struct Pt
{
    float x;   // 0~1
    float y;   // 0~1
};

class CurveCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit CurveCanvas(QWidget* parent = nullptr);

    void setPoints(const std::vector<Pt>& p);
    std::vector<Pt> points() const { return _pts; }

signals:
    // ★ UI 层事件，不带数据
    void curveChanged();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QPointF toCanvas(const Pt& p) const;
    Pt toPoint(const QPointF& p) const;
    int pickPoint(const QPointF& pos) const;

    float catmull(float p0, float p1, float p2, float p3, float t) const;

private:
    std::vector<Pt> _pts;
    int _activePoint = -1;
    const float _pickRadius = 0.04f;
};
