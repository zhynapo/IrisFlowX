#pragma once
#include <QWidget>
#include <QMap>
#include <QJsonObject>
#include <QTimer>
#include "core/NodeDesc.h"
#include "BaseGraphModel.hpp"
//#include "Global.h"

using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeDataType;

namespace Flow {
    class FLOWCORE_API BaseNodeModel : public QtNodes::NodeDelegateModel
    {
        Q_OBJECT
    public:
        BaseNodeModel() = default;
        BaseNodeModel(const NodeDesc& d);
        virtual ~BaseNodeModel() = default;

        // 必须 override 的接口
        QString caption() const override;
        QString name() const override;

        unsigned int nPorts(QtNodes::PortType type) const override;
        QtNodes::NodeDataType dataType(QtNodes::PortType type,
            QtNodes::PortIndex index) const override;

        QString portCaption(QtNodes::PortType type,
            QtNodes::PortIndex index) const override;

        void setInData(std::shared_ptr<QtNodes::NodeData> data,
            QtNodes::PortIndex portIndex) override;
        // QtNodes 要求的输出数据接口
        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex index) override;
        virtual std::shared_ptr<QtNodes::NodeData> previewData(int portIndex);

        // UI 生成
        QWidget* embeddedWidget() override;

        // BaseNodeModel.h
        void setParamRange(const QString& name, double min, double max);
        void setParamEnabled(const QString& name, bool enabled);
        void setParamEnumOptions(const QString& name, const QStringList& opts);
        void applyRuntimeToUI(const QString& name);

        // save/load
        QJsonObject save() const override;
        void load(QJsonObject const& obj) override;
        void setParamJson(const QString& name, const QJsonObject& obj);
        QJsonObject getParamJson(const QString& name) const;

        // 自动管理参数
        //virtual QVector<ParamDesc> parameters() const { return {}; }
        const QVector<ParamDesc>& params() const { return _desc.params; }
        virtual QVariant parameterValue(const QString& name) const;
        virtual void setParameter(const QString& name, const QVariant& v);
        virtual void applyParameter(const QString& name, const QVariant& v);

        void setGraphModel(BaseGraphModel* gm) {
            qDebug() << "graphModel injected!" << gm;

            _graphModel = gm;
        }
        void evaluateManually()
        {
            //if (GlobalGraphState::LOADING )
            //    return;

            process();
            emitOutputs();
        }

    signals:
        void paramRuntimeChanged(const QString& name);

    protected:
        void emitOutputs();
        // 子类实现处理逻辑
        void update() {
            //if (!_paramUpdateTimer)
            //    return; // 没初始化就跳过，不算
            _paramUpdateTimer->start();
        };
        void proc_() {
            process();
            emitOutputs();
        }

        // 子类实现处理逻辑
        virtual void process() {}
        // 子类用于设置输出
        void setOutputData(QtNodes::PortIndex index,
            std::shared_ptr<QtNodes::NodeData> data);
        std::shared_ptr<QtNodes::NodeData> _getInput(QtNodes::PortIndex index) const;

        BaseGraphModel* graphModel() const { return _graphModel; }

        void initTimer();

    private:
        BaseGraphModel* findGraphModel(QObject* o);

    protected:
        QWidget* _widget = nullptr;
        QHash<QString, IParamEditor*> _editors;
        QHash<QString, QVariant> _paramValues;

    protected:
        NodeDesc _desc;
        std::vector<std::shared_ptr<QtNodes::NodeData>> _outputData;
        std::vector<std::shared_ptr<QtNodes::NodeData>> _inputData;
        QHash<QString, ParamRuntime> _paramRuntime;

    private:
        BaseGraphModel* _graphModel = nullptr;
        QTimer* _paramUpdateTimer = nullptr;

    };
}

