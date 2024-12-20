#pragma once
#include <QWidget>
#include <QColor>
#include <QLabel>
#include <QEvent>
#include <QHBoxLayout>
#include <QColorDialog>

class ColorPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPickerWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        _label = new QLabel(this);
        _label->setFixedSize(28, 18);
        _label->setFrameShape(QFrame::Box);
        _label->setLineWidth(1);
        _label->setAutoFillBackground(true);

        auto layout = new QHBoxLayout(this);
        layout->setContentsMargins(2, 2, 2, 2);
        layout->addWidget(_label);

        setColor(Qt::red);

        _label->installEventFilter(this);

        // 创建独立的颜色对话框（非模态）
        _dialog = new QColorDialog(nullptr);
        _dialog->setWindowFlag(Qt::Tool);          // 防止产生“黑框父窗口”
        _dialog->setOption(QColorDialog::NoButtons);
        _dialog->setModal(false);                  // 重要：禁用模态
        _dialog->hide();

        connect(_dialog, &QColorDialog::currentColorChanged,
            this, &ColorPickerWidget::setColor);
    }

    QColor color() const { return _color; }

signals:
    void colorChanged(const QColor& c);

public slots:
    void setColor(const QColor& c)
    {
        _color = c;

        QPalette pal = _label->palette();
        pal.setColor(QPalette::Window, c);
        _label->setPalette(pal);

        emit colorChanged(c);
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if (obj == _label && event->type() == QEvent::MouseButtonPress)
        {
            _dialog->setCurrentColor(_color);
            _dialog->show();
            _dialog->raise();
            _dialog->activateWindow();
            return true;
        }
        return QWidget::eventFilter(obj, event);
    }

private:
    QLabel* _label;
    QColor _color;
    QColorDialog* _dialog;
};
