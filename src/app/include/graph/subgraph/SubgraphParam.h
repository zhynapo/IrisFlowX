#pragma once
#include <QtNodes/Definitions>
#include <QString>
#include <QVariant>
#include <vector>
#include <core/FlowTypes.h>

struct ExposedParam
{
    // 哪个内部节点暴露出来的参数
    QtNodes::NodeId internalNode;        // 内部节点 ID
    QString name;               // 内部参数名
    QString label;              // UI 显示 label

    Flow::ParamUIType uiType;         // Int/Double/Bool/Combo
    QVariant defaultValue;

    QVector<QString> comboNames;    // 仅 uiType=Combo 有效
    QVector<int> comboValues;       // 仅 uiType=Combo 有效
};
