#pragma once
#include "factory/IParamEditor.h"
#include "widgets/ParamEditor.h"
namespace Flow
{

    class ParamEditorFactory
    {
    public:
        static IParamEditor* create(const ParamDesc& p)
        {
            switch (p.type)
            {
            case ParamUIType::Int:
                return new IntParamEditor(
                    p.defaultValue.toInt(),
                    (int)p.minValue,
                    (int)p.maxValue,
                    (int)p.step
                );

            case ParamUIType::Double:
                return new DoubleParamEditor(
                    p.defaultValue.toDouble(),
                    p.minValue,
                    p.maxValue,
                    p.step,
                    3
                );

            case ParamUIType::Bool:
                return new BoolParamEditor(p.defaultValue.toBool());

            case ParamUIType::Combo:
            {
                int defaultValue = p.defaultValue.toInt();
                return new ComboParamEditor(
                    p.comboNames,
                    p.comboValues,
                    defaultValue
                );
            }

            case ParamUIType::Color:
                return new ColorParamEditor(p.defaultValue.value<QColor>());

            case ParamUIType::Curve:
            {
                QJsonObject def = QJsonObject::fromVariantMap(
                    p.defaultValue.toMap()
                );
                return new CurveParamEditor(def);
            }
            }

            return nullptr;
        }
    };
}