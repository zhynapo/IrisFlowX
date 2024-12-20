// include/core/NodeDesc.hpp
#pragma once
#include "FlowCoreExport.h"
#include "FlowTypes.h"

namespace Flow {
    struct FLOWCORE_API PortDesc {
        QString name;
        QtNodes::NodeDataType type;

        static PortDesc In(QString n, QtNodes::NodeDataType t)  { return { n, t }; }
        static PortDesc Out(QString n, QtNodes::NodeDataType t) { return { n, t }; }
    };

    struct FLOWCORE_API NodeDesc {
        QString modelName;   // 内部唯一 ID (如 "ImageBlur")
        QString displayName; // 界面显示名称 (如 "Gaussian Blur")
        QString category;    // 菜单分类 (如 "Image/Filters")
        uint32_t version = 1;        
        QVector<PortDesc> inputs;
        QVector<PortDesc> outputs;
        QVector<ParamDesc> params;

        // --- 便捷构造函数 ---
        NodeDesc() = default;

        NodeDesc(QString id, QString name, QString cat)
            : modelName(id), displayName(name), category(cat) {}

        // 补一个全参数构造函数
        NodeDesc(QString id, QString name, QString cat,
            QVector<PortDesc> ins, QVector<PortDesc> outs, QVector<ParamDesc> pars)
            : modelName(id), displayName(name), category(cat),
            inputs(ins), outputs(outs), params(pars) {
        }

        // 链式调用：让定义更简洁
        NodeDesc& addInput(QString name, QtNodes::NodeDataType type) {
            inputs.append(PortDesc::In(name, type));
            return *this;
        }

        NodeDesc& addOutput(QString name, QtNodes::NodeDataType type) {
            outputs.append(PortDesc::Out(name, type));
            return *this;
        }

        NodeDesc& addParam(const ParamDesc& p) {
            params.append(p);
            return *this;
        }
    };
}