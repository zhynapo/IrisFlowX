#pragma once
#include <QString>
#include <QVariant>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include "FlowCoreExport.h"
#include <QtNodes/NodeDelegateModel>


namespace Flow {

    enum class ParamUIType {
        Int,
        Double,
        Bool,
        Combo,
        String,
        Color,
        Json,
        Curve
    };

    struct FLOWCORE_API ParamDesc
    {
        QString name;        // internal name
        QString label;       // UI label
        ParamUIType type;
        QVariant defaultValue;

        // for combo
        QVector<QString> comboNames;
        QVector<int> comboValues;

        // for numeric types
        double minValue = 0.0;
        double maxValue = 100.0;
        double step = 1.0;
        int decimals = 3;

        // -------------------------------
        // Factory methods
        // -------------------------------

        static ParamDesc makeInt(QString name, QString label, int def)
        {
            ParamDesc p{ name, label, ParamUIType::Int, def };
            p.minValue = -999999;
            p.maxValue = 999999;
            p.step = 1.0;
            return p;
        }

        static ParamDesc makeDouble(QString name, QString label, double def)
        {
            ParamDesc p{ name, label, ParamUIType::Double, def };
            p.minValue = -999999;
            p.maxValue = 999999;
            p.step = 0.1;
            p.decimals = 6;
            return p;
        }

        static ParamDesc makeBool(QString name, QString label, bool def)
        {
            return { name, label, ParamUIType::Bool, def };
        }

        static ParamDesc makeString(QString name, QString label, QString def)
        {
            return { name, label, ParamUIType::String, def };
        }

        static ParamDesc makeCombo(QString name, QString label,
            QVector<QString> names,
            QVector<int> values,
            int def)
        {
            ParamDesc p{ name, label, ParamUIType::Combo, def };
            p.comboNames = names;
            p.comboValues = values;
            return p;
        }

        static ParamDesc makeColor(QString name, QString label, QColor def)
        {
            ParamDesc p{ name, label, ParamUIType::Color, def };
            return p;
        }

        static ParamDesc makeJson(QString name, QString label, QJsonObject def = QJsonObject())
        {
            ParamDesc p{ name, label, ParamUIType::Json, def.toVariantMap() };
            return p;
        }

        // ParamDesc.hpp
        static ParamDesc makeCurve(const QString& name, const QString& label)
        {
            ParamDesc p;
            p.name = name;
            p.label = label;
            p.type = ParamUIType::Curve;

            // ===== 默认曲线：Linear =====
            QJsonArray arr;
            {
                QJsonObject p0; p0["x"] = 0.0; p0["y"] = 0.0;
                QJsonObject p1; p1["x"] = 1.0; p1["y"] = 1.0;
                arr.append(p0);
                arr.append(p1);
            }

            QJsonObject curve;
            curve["points"] = arr;

            // 存成 QVariant（⚠ Node 参数系统统一用 QVariant）
            p.defaultValue = curve.toVariantMap();

            return p;
        }

        // -------------------------------
        // Fluent setters
        // -------------------------------

        ParamDesc& setRange(double minV, double maxV)
        {
            minValue = minV;
            maxValue = maxV;
            return *this;
        }

        ParamDesc& setStep(double s)
        {
            step = s;
            return *this;
        }

        ParamDesc& setDecimals(int d)
        {
            decimals = d;
            return *this;
        }
    };

    // ParamRuntime.h
    struct FLOWCORE_API ParamRuntime
    {
        bool enabled = true;

        // ---- 数值类约束 ----
        std::optional<double> min;
        std::optional<double> max;
        std::optional<double> step;
        std::optional<int>    decimals;

        // ---- 枚举类 ----
        std::optional<QStringList> enumOptions;

        // ---- 字符串类 ----
        std::optional<QString> placeholder;
        std::optional<QString> regex;

        // ---- 复合类型 ----
        QVariant extra;   // ROI / Size / 自定义扩展
    };

}