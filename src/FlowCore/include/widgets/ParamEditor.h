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
        IntParamEditor(QString labelStr, int v, int minV, int maxV, int step, QWidget* parent = nullptr)
            : QWidget(parent), step(step)
        {
            // Main vertical layout for the entire widget
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            // Create the label
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            // Panel for the actual controls
            panel = new QWidget(this);
            auto* layout = new QHBoxLayout(panel);
            layout->setContentsMargins(0, 0, 0, 0);

            sp = new QSpinBox();
            sl = new QSlider(Qt::Horizontal);

            sp->setRange(minV, maxV);
            sl->setRange(minV, maxV);

            sp->setSingleStep(step);
            sl->setSingleStep(step);   // 只对键盘有效，对拖动无效

            sp->setValue(v);
            sl->setValue(v);

            layout->addWidget(sp);
            layout->addWidget(sl);

            mainLayout->addWidget(panel);

            //--------------------------------------------------
            // Internal Sync (Slider → SpinBox) with step enforcing
            //--------------------------------------------------
            connect(sl, &QSlider::valueChanged, this, [this](int v){
                int fixed = alignToStep(v);
                if (fixed != v)
                {
                    sl->blockSignals(true);
                    sl->setValue(fixed);
                    sl->blockSignals(false);
                }

                sp->blockSignals(true);
                sp->setValue(fixed);
                sp->blockSignals(false);

                if (onValueChanged) onValueChanged(fixed);
            });

            //--------------------------------------------------
            // Internal Sync (SpinBox → Slider) with step enforcing
            //--------------------------------------------------
            connect(sp, qOverload<int>(&QSpinBox::valueChanged), this, [this](int v){
                int fixed = alignToStep(v);
                if (fixed != v)
                {
                    sp->blockSignals(true);
                    sp->setValue(fixed);
                    sp->blockSignals(false);
                }

                sl->blockSignals(true);
                sl->setValue(fixed);
                sl->blockSignals(false);

                if (onValueChanged) onValueChanged(fixed);
            });
        }

        QWidget* widget() override 
        { 
            return this; 
        }

        //------------------------------------------------------
        // Runtime UI update: range / step / enable
        //------------------------------------------------------
        void applyRuntime(const ParamRuntime& rt) override
        {
            if (rt.min && rt.max)
            {
                sp->setRange((int)*rt.min, (int)*rt.max);
                sl->setRange((int)*rt.min, (int)*rt.max);
            }

            if (rt.step)
            {
                step = std::max(1, (int)*rt.step);
                sp->setSingleStep(step);
                // slider 无法做到拖动步进，只能在 valueChanged 手动控制
            }

            this->setEnabled(rt.enabled);
        }

        //------------------------------------------------------
        // Model → UI
        //------------------------------------------------------
        void setValue(const QVariant& v) override
        {
            int val = alignToStep(v.toInt());

            sp->blockSignals(true);
            sl->blockSignals(true);

            sp->setValue(val);
            sl->setValue(val);

            sp->blockSignals(false);
            sl->blockSignals(false);
        }

    private:

        //------------------------------------------------------
        // Ensures the value respects the step constraint
        //------------------------------------------------------
        int alignToStep(int v) const
        {
            if (step <= 1) return v;
            int base = sp->minimum();
            return base + ((v - base) / step) * step;
        }

    private:
        QLabel* label = nullptr;
        QWidget* panel = nullptr;
        QSpinBox* sp = nullptr;
        QSlider* sl = nullptr;
        int step = 1;
    };

    // ========================================================================
    // Double 参数编辑器
    // ========================================================================
    class DoubleParamEditor : public QWidget, public IParamEditor
    {
    public:
        DoubleParamEditor(QString labelStr, double v, double minV, double maxV,
            double step, int decimals = 3,
            QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            // Main vertical layout for the entire widget
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            // Create the label
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            sp = new QDoubleSpinBox(this);
            sp->setRange(minV, maxV);
            sp->setSingleStep(step);
            sp->setDecimals(decimals);
            sp->setValue(v);

            mainLayout->addWidget(sp);

            connect(sp, qOverload<double>(&QDoubleSpinBox::valueChanged),
                this, [this](double v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(v);
                });
        }

        QWidget* widget() override { return this; }

        void applyRuntime(const ParamRuntime& rt) override
        {
            if (rt.min && rt.max)
                sp->setRange(*rt.min, *rt.max);

            if (rt.step)
                sp->setSingleStep(*rt.step);

            if (rt.decimals)
                sp->setDecimals(*rt.decimals);

            this->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            sp->setValue(v.toDouble());
        }

    private:
        QLabel* label = nullptr;
        QDoubleSpinBox* sp = nullptr;
    };

    // ========================================================================
    // Bool 参数编辑器（最终版）
    // ========================================================================
    class BoolParamEditor : public QWidget, public IParamEditor
    {
    public:
        explicit BoolParamEditor(QString labelStr, bool v, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            cb = new QCheckBox(this);
            cb->setChecked(v);
            
            mainLayout->addWidget(cb);

            // UI → Model（统一出口）
            connect(cb, &QCheckBox::toggled,
                this, [this](bool v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(v);
                });
        }

        QWidget* widget() override
        {
            return this;
        }

        void applyRuntime(const ParamRuntime& rt) override
        {
            this->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            cb->blockSignals(true);
            cb->setChecked(v.toBool());
            cb->blockSignals(false);
        }

    private:
        QLabel* label = nullptr;
        QCheckBox* cb = nullptr;
    };


    // ========================================================================
    // Combo 参数编辑器（最终版）
    // ========================================================================
    class ComboParamEditor : public QWidget, public IParamEditor
    {
    public:
        ComboParamEditor(QString labelStr,
                        const QStringList& names,
                        const QList<int>& values,
                        int currentValue,
                        QWidget* parent = nullptr)
            : QWidget(parent)
        {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            combo = new QComboBox(this);

            // 构建带 itemData 的列表（永远正确）
            for (int i = 0; i < names.size(); ++i)
                combo->addItem(names[i], values[i]);

            // 设置初始值
            int idx = combo->findData(currentValue);
            combo->setCurrentIndex(idx >= 0 ? idx : 0);
            
            mainLayout->addWidget(combo);

            // UI → Model （永远传递 itemData，而不是 index）
            connect(combo, qOverload<int>(&QComboBox::currentIndexChanged),
                this, [this](int){
                    if (onValueChanged)
                        onValueChanged(combo->currentData());
                });
        }

        QWidget* widget() override
        {
            return this;
        }

        // 运行时修改 UI 外观，但不修改原始 itemData
        void applyRuntime(const ParamRuntime& rt) override
        {
            combo->blockSignals(true);

            if (rt.enumOptions.has_value())
            {
                // 仅更新名称，不影响内部 value（itemData）
                auto names = rt.enumOptions.value();

                int n = std::min(int(names.size()), combo->count());
                for (int i = 0; i < n; ++i)
                    combo->setItemText(i, names[i]);
            }

            this->setEnabled(rt.enabled);
            combo->blockSignals(false);
        }

        // Model → UI，用真实值定位
        void setValue(const QVariant& v) override
        {
            combo->blockSignals(true);
            int idx = combo->findData(v);
            if (idx >= 0)
                combo->setCurrentIndex(idx);
            combo->blockSignals(false);
        }

    private:
        QLabel* label = nullptr;
        QComboBox* combo = nullptr;
    };

    // ========================================================================
    // Color 参数编辑器（你的 ColorPickerWidget）
    // ========================================================================
    class ColorParamEditor : public QWidget, public IParamEditor
    {
    public:
        ColorParamEditor(QString labelStr, const QColor& c, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            cp = new ColorPickerWidget(this);
            cp->setColor(c);
            
            mainLayout->addWidget(cp);

            connect(cp, &ColorPickerWidget::colorChanged,
                this, [this](const QColor& v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(c);
                });
        }

        QWidget* widget() override { return this; }

        void applyRuntime(const ParamRuntime& rt) override
        {
            this->setEnabled(rt.enabled);
        }

        void setValue(const QVariant& v) override
        {
            cp->setColor(v.value<QColor>());
        }

    private:
        QLabel* label = nullptr;
        ColorPickerWidget* cp = nullptr;
    };

    // ========================================================================
    // Curve 参数编辑器（你的 ）
    // ========================================================================
    class CurveParamEditor : public QWidget, public IParamEditor
    {
    public:
        explicit CurveParamEditor(QString labelStr, const QJsonObject& defaultCurve, QWidget* parent = nullptr)
            //: IParamEditor(parent)
        {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            
            label = new QLabel(labelStr, this);
            mainLayout->addWidget(label);
            
            _editor = new CurveEditorWidget(this);
            _editor->setCurve(defaultCurve);
            
            mainLayout->addWidget(_editor);

            connect(_editor, &CurveEditorWidget::curveChanged,
                this, [this](const QJsonObject& v) {
                    if (onValueChanged) onValueChanged(v); //emit valueChanged(obj.toVariantMap());
                });
        }

        QWidget* widget() override
        {
            return this;
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
            this->setEnabled(rt.enabled);
        }

    private:
        QLabel* label = nullptr;
        CurveEditorWidget* _editor = nullptr;
    };

    // ========================================================================
    // Label 参数编辑器（最终版）
    // ========================================================================
    class LabelParamEditor : public QWidget, public IParamEditor
    {
    public:
        explicit LabelParamEditor(QString text, QWidget* parent = nullptr)
        {
            // Create a horizontal layout to hold the label and the value
            QHBoxLayout* layout = new QHBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            
            // Create the label part - use the text as the label name
            labelPart = new QLabel(text + " : ", parent);
            labelPart->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; padding: 3px; }");
            
            // Create the value part - initially empty
            valuePart = new QLabel("", parent);
            valuePart->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; padding: 3px; }");
            
            // Add both parts to the layout
            layout->addWidget(labelPart);
            layout->addWidget(valuePart);
            
            // Make the layout expand to fill the widget
            layout->addStretch();
        }

        QWidget* widget() override
        {
            return this;  // Return this widget which contains both label and value
        }

        void applyRuntime(const ParamRuntime& rt) override
        {
            // Labels are typically always enabled
            this->setEnabled(true);
        }

        void setValue(const QVariant& v) override
        {
            // Update only the value part
            valuePart->setText(v.toString());
        }

    private:
        QLabel* labelPart = nullptr;
        QLabel* valuePart = nullptr;
    };

}