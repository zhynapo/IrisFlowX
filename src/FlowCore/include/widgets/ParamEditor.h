#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QColor>
#include <optional>
#include "factory/IParamEditor.h"
#include "core/FlowTypes.h"
#include "widgets/ColorPickerWidget.hpp"
#include "widgets/CurveEditorWidget.h"

namespace Flow
{
    // ========================================================================
    // Int 参数编辑器（SpinBox + Slider）
    // ========================================================================
    class IntParamEditor : public QWidget, public IParamEditor
    {
    public:
        IntParamEditor(int v, int minV, int maxV, int step, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            panel = new QWidget(parent);
            auto* layout = new QHBoxLayout(panel);
            layout->setContentsMargins(0, 0, 0, 0);

            sp = new QSpinBox();
            sl = new QSlider(Qt::Horizontal);

            sp->setRange(minV, maxV);
            sl->setRange(minV, maxV);
            sp->setSingleStep(step);
            sl->setSingleStep(step);

            sp->setValue(v);
            sl->setValue(v);

            layout->addWidget(sp);
            layout->addWidget(sl);

            // 内部同步
            connect(sp, qOverload<int>(&QSpinBox::valueChanged),
                sl, &QSlider::setValue);
            connect(sl, &QSlider::valueChanged,
                sp, &QSpinBox::setValue);

            // UI → Model（统一出口）
            connect(sp, qOverload<int>(&QSpinBox::valueChanged),
                this, [this](int v) {
                    if (onValueChanged) onValueChanged(v);//emit valueChanged(v);
                });
        }

        QWidget* widget() override { return panel; }

        void applyRuntime(const ParamRuntime& rt) override
        {
            if (rt.min && rt.max)
            {
                sp->setRange((int)*rt.min, (int)*rt.max);
                sl->setRange((int)*rt.min, (int)*rt.max);
            }
            if (rt.step)
            {
                int st = std::max(1, (int)*rt.step);
                sp->setSingleStep(st);
                sl->setSingleStep(st);
            }
            panel->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            int i = v.toInt();
            sp->setValue(i);
            sl->setValue(i);
        }

    private:
        QWidget* panel = nullptr;
        QSpinBox* sp = nullptr;
        QSlider* sl = nullptr;
    };

    // ========================================================================
    // Double 参数编辑器
    // ========================================================================
    class DoubleParamEditor : public QWidget, public IParamEditor
    {
    public:
        DoubleParamEditor(double v, double minV, double maxV,
            double step, int decimals = 3,
            QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            sp = new QDoubleSpinBox(parent);
            sp->setRange(minV, maxV);
            sp->setSingleStep(step);
            sp->setDecimals(decimals);
            sp->setValue(v);

            connect(sp, qOverload<double>(&QDoubleSpinBox::valueChanged),
                this, [this](double v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(v);
                });
        }

        QWidget* widget() override { return sp; }

        void applyRuntime(const ParamRuntime& rt) override
        {
            if (rt.min && rt.max)
                sp->setRange(*rt.min, *rt.max);

            if (rt.step)
                sp->setSingleStep(*rt.step);

            if (rt.decimals)
                sp->setDecimals(*rt.decimals);

            sp->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            sp->setValue(v.toDouble());
        }

    private:
        QDoubleSpinBox* sp = nullptr;
    };

    // ========================================================================
    // Bool 参数编辑器（最终版）
    // ========================================================================
    class BoolParamEditor : public QWidget, public IParamEditor
    {
    public:
        explicit BoolParamEditor(bool v, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            cb = new QCheckBox(parent);
            cb->setChecked(v);

            // UI → Model（统一出口）
            connect(cb, &QCheckBox::toggled,
                this, [this](bool v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(v);
                });
        }

        QWidget* widget() override
        {
            return cb;
        }

        void applyRuntime(const ParamRuntime& rt) override
        {
            cb->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            cb->setChecked(v.toBool());
        }

    private:
        QCheckBox* cb = nullptr;
    };


    // ========================================================================
    // Combo 参数编辑器（最终版）
    // ========================================================================
    class ComboParamEditor : public QWidget, public IParamEditor
    {
    public:
        ComboParamEditor(const QStringList& names,
            const QList<int>& values,
            int currentValue,
            QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            combo = new QComboBox(parent);

            for (int i = 0; i < names.size(); ++i)
            {
                combo->addItem(names[i], values[i]);
            }

            int idx = combo->findData(currentValue);
            combo->setCurrentIndex(idx >= 0 ? idx : 0);

            // UI → Model（统一出口）
            connect(combo, qOverload<int>(&QComboBox::currentIndexChanged),
                this, [this](int v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(combo->itemData(idx));
                });
        }

        QWidget* widget() override
        {
            return combo;
        }

        void applyRuntime(const ParamRuntime& rt) override
        {
            if (rt.enumOptions.has_value())
            {
                combo->blockSignals(true);

                QStringList names = rt.enumOptions.value();
                combo->clear();

                // Runtime 下：默认 value = index
                for (int i = 0; i < names.size(); ++i)
                {
                    combo->addItem(names[i], i);
                }

                combo->blockSignals(false);
            }

            combo->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            int idx = combo->findData(v);
            if (idx >= 0)
                combo->setCurrentIndex(idx);
        }

    private:
        QComboBox* combo = nullptr;
    };

    // ========================================================================
    // Color 参数编辑器（你的 ColorPickerWidget）
    // ========================================================================
    class ColorParamEditor : public QWidget, public IParamEditor
    {
    public:
        ColorParamEditor(const QColor& c, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            cp = new ColorPickerWidget(parent);
            cp->setColor(c);

            connect(cp, &ColorPickerWidget::colorChanged,
                this, [this](const QColor& v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(c);
                });
        }

        QWidget* widget() override { return cp; }

        void applyRuntime(const ParamRuntime& rt) override
        {
            cp->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            cp->setColor(v.value<QColor>());
        }

    private:
        ColorPickerWidget* cp = nullptr;
    };

    // ========================================================================
    // Curve 参数编辑器（你的 ）
    // ========================================================================
    class CurveParamEditor : public QWidget, public IParamEditor
    {
    public:
        explicit CurveParamEditor(const QJsonObject& defaultCurve, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            _editor = new CurveEditorWidget(parent);
            _editor->setCurve(defaultCurve);

            connect(_editor, &CurveEditorWidget::curveChanged,
                this, [this](const QJsonObject& v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(obj.toVariantMap());
                });
        }

        QWidget* widget() override
        {
            return _editor;
        }

        void setValue(const QVariant& v) override
        {
            QJsonObject obj = QJsonObject::fromVariantMap(v.toMap());
            _editor->setCurve(obj);
        }

        void applyRuntime(const ParamRuntime& rt) override
        {
            // Curve 目前不需要 runtime 控制
            // 但为了接口完整性，必须实现
            _editor->setEnabled(rt.enabled);
        }

    private:
        CurveEditorWidget* _editor = nullptr;
    };
}