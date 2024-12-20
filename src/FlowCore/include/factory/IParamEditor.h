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
#include "core/FlowTypes.h"

namespace Flow 
{
    // 统一接口：所有参数编辑控件都实现这个
    struct IParamEditor
    {
        virtual ~IParamEditor() = default;

        virtual QWidget* widget() = 0;
        virtual void setValue(const QVariant& v) = 0;
        virtual void applyRuntime(const ParamRuntime& rt) = 0;

        std::function<void(const QVariant&)> onValueChanged;
    };
}