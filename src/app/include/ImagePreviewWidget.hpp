#pragma once
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QWheelEvent>

class ZoomGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;

protected:
    void wheelEvent(QWheelEvent* e) override
    {
        double scaleFactor = std::pow(1.0015, e->angleDelta().y());
        scale(scaleFactor, scaleFactor);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            _panning = true;
            _lastPos = e->pos();
            setCursor(Qt::ClosedHandCursor);
            e->accept();
            return;
        }
        QGraphicsView::mousePressEvent(e);
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (_panning) {
            auto delta = mapToScene(e->pos()) - mapToScene(_lastPos);
            translate(delta.x(), delta.y());
            _lastPos = e->pos();
            e->accept();
            return;
        }
        QGraphicsView::mouseMoveEvent(e);
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        if (_panning && e->button() == Qt::LeftButton) {
            _panning = false;
            setCursor(Qt::ArrowCursor);
            e->accept();
            return;
        }
        QGraphicsView::mouseReleaseEvent(e);
    }

private:
    bool _panning = false;
    QPoint _lastPos;
};

class ImagePreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImagePreviewWidget(QWidget* parent = nullptr);
    void setPixmap(const QPixmap& pm);

private:
    QGraphicsScene* _scene;
    ZoomGraphicsView* _view;
};
