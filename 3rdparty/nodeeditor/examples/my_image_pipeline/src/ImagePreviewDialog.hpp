#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QWheelEvent>
#include <QtGui/QMouseEvent>

class ZoomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;
protected:
    void wheelEvent(QWheelEvent* e) override
    {
        const double factor = std::pow(1.0015, e->angleDelta().y());
        scale(factor, factor);
    }
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton || e->button() == Qt::MiddleButton) {
            _panning = true; _last = e->pos(); setCursor(Qt::ClosedHandCursor); e->accept(); return;
        }
        QGraphicsView::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (_panning) {
            QPointF d = mapToScene(e->pos()) - mapToScene(_last);
            translate(d.x(), d.y());
            _last = e->pos();
            e->accept(); return;
        }
        QGraphicsView::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override
    {
        if (_panning && (e->button()==Qt::LeftButton || e->button()==Qt::MiddleButton)) {
            _panning=false; setCursor(Qt::ArrowCursor); e->accept(); return;
        }
        QGraphicsView::mouseReleaseEvent(e);
    }
private:
    bool _panning=false;
    QPoint _last;
};

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImagePreviewDialog(QWidget* parent=nullptr);

    void setPixmap(const QPixmap& pm);

private:
    QGraphicsScene* _scene = nullptr;
    ZoomGraphicsView* _view = nullptr;
};