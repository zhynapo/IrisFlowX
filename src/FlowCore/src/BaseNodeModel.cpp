
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "model/Common.hpp"
#include "model/BaseNodeModel.hpp"


namespace Flow {
    // ========================================
    // 有参构造：由子类调用
    // ========================================
    BaseNodeModel::BaseNodeModel(const NodeDesc& d)
        : _desc(d)
    {
        for (auto& p : _desc.params)
            _paramValues[p.name] = p.defaultValue;
        _outputData.resize(_desc.outputs.size());
        _inputData.resize(_desc.inputs.size());

        initTimer();

    }

    void BaseNodeModel::initTimer()
    {
        //_graphModel = findGraphModel(this);
        _paramUpdateTimer = new QTimer(this);
        _paramUpdateTimer->setSingleShot(true);
        _paramUpdateTimer->setInterval(150); // 100~300ms 很合适

        connect(_paramUpdateTimer, &QTimer::timeout, this, [this]() {
            this->proc_();   // 真正触发一次计算
            });
    }

    QString BaseNodeModel::caption() const
    {
        return _desc.displayName;
    }

    QString BaseNodeModel::name() const
    {
        return _desc.modelName;
    }

    unsigned int BaseNodeModel::nPorts(QtNodes::PortType type) const
    {
        if (type == QtNodes::PortType::In)
            return _desc.inputs.size();
        return _desc.outputs.size();
    }

    QtNodes::NodeDataType BaseNodeModel::dataType(QtNodes::PortType type,
        QtNodes::PortIndex index) const
    {
        return (type == QtNodes::PortType::In)
            ? _desc.inputs[index].type
            : _desc.outputs[index].type;
    }

    QString BaseNodeModel::portCaption(QtNodes::PortType type,
        QtNodes::PortIndex index) const
    {
        const QString& name =
            (type == QtNodes::PortType::In)
            ? _desc.inputs[index].name
            : _desc.outputs[index].name;

        return name;
    }

    void BaseNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
        QtNodes::PortIndex portIndex)
    {
        if (_graphModel && _graphModel->isLoading())
            return;

        qDebug() << "[setInData]" << caption()
            << "| port =" << portIndex;

        // 确保 _inputData 大小与输入端口数量一致
        if (_inputData.size() != _desc.inputs.size())
            _inputData.resize(_desc.inputs.size());

        _inputData[portIndex] = data;

        // 输入更新后调用 update()
        // ★★★ 加载期间禁止触发 update/process ★★★
        update();
    }

    std::shared_ptr<QtNodes::NodeData> BaseNodeModel::_getInput(QtNodes::PortIndex index) const
    {
        if (index < 0 || index >= _inputData.size()) return nullptr;
        return _inputData[index];
    }

    std::shared_ptr<QtNodes::NodeData> BaseNodeModel::outData(QtNodes::PortIndex index)
    {
        if (index < 0 || index >= _outputData.size()) return nullptr;
        return _outputData[index];
    }

    void BaseNodeModel::setOutputData(QtNodes::PortIndex index,
        std::shared_ptr<QtNodes::NodeData> data)
    {
        if (_outputData.size() != _desc.outputs.size())
            _outputData.resize(_desc.outputs.size());

        _outputData[index] = data;
    }

    QWidget* BaseNodeModel::embeddedWidget()
    {
        if (_widget)
            return _widget;

        _widget = new QWidget();
        auto* layout = new QVBoxLayout(_widget);
        layout->setContentsMargins(0, 0, 0, 0);

        for (auto& p : _desc.params)
        {
            QString paramName = p.name;

            // -------- 创建编辑器控件（统一工厂） --------
            //IParamEditor* ed = ParamEditorFactory::create(p);
            IParamEditor* ed = nullptr;
            if (_graphModel && _graphModel->paramEditorFactory())
                ed = _graphModel->paramEditorFactory()->create(p);

            _editors[paramName] = ed;
            _paramRuntime[p.name] = ParamRuntime();  // 初始化默认状态

            // -------- 添加控件到 UI --------
            QWidget* w = ed->widget();
            layout->addWidget(w);

            // -------- 记录默认参数值 --------

            // -------- 记录默认参数值 --------
            _paramValues[paramName] = p.defaultValue;

            // -------- 链接 UI → 参数变化 --------
            ed->onValueChanged = [this, paramName](const QVariant& v) {
                this->setParameter(paramName, v);
                };
            //connect(ed, &IParamEditor::valueChanged,
            //    this, [this, paramName](const QVariant& v) {
            //        setParameter(paramName, v);
            //    });
        }
        layout->addStretch();
        return _widget;
    }

    void BaseNodeModel::setParamRange(const QString& name, double minV, double maxV)
    {
        ParamRuntime& rt = _paramRuntime[name];
        rt.min = minV;
        rt.max = maxV;
        applyRuntimeToUI(name);
    }

    void BaseNodeModel::setParamEnabled(const QString& name, bool en)
    {
        ParamRuntime& rt = _paramRuntime[name];
        rt.enabled = en;
        applyRuntimeToUI(name);
    }

    void BaseNodeModel::setParamEnumOptions(const QString& name, const QStringList& opts)
    {
        ParamRuntime& rt = _paramRuntime[name];
        rt.enumOptions = opts;
        applyRuntimeToUI(name);
    }

    void BaseNodeModel::applyRuntimeToUI(const QString& name)
    {
        if (!_editors.contains(name))
            return;

        const ParamRuntime& rt = _paramRuntime[name];
        _editors[name]->applyRuntime(rt);
    }

    QJsonObject BaseNodeModel::save() const
    {
        // 1. 先让 QtNodes 保存它需要的内容（包含 model-name）
        QJsonObject obj = NodeDelegateModel::save();
        for (auto& p : _desc.params)
        {
            QVariant v = _paramValues.value(p.name);

            if (p.type == ParamUIType::Json)
            {
                QVariantMap map = v.toMap();
                obj[p.name] = QJsonObject::fromVariantMap(map);
                qDebug() << QJsonObject::fromVariantMap(map);
            }
            else
            {
                obj[p.name] = QJsonValue::fromVariant(v);
            }
        }
        return obj;
    }

    void BaseNodeModel::load(QJsonObject const& obj)
    {
        for (auto& p : _desc.params)
        {
            if (!obj.contains(p.name))
                continue;

            QVariant v = obj[p.name].toVariant();

            if (p.type == ParamUIType::Json)
            {
                QJsonObject jo = obj[p.name].toObject();
                v = jo.toVariantMap();
            }
            else {
                // ---- 自动类型转换 ----
                if (p.type == ParamUIType::Color)
                {
                    // JSON 给的是 "#rrggbb"
                    QString s = v.toString();
                    QColor c(s);
                    v = c;
                }
                else if (p.type == ParamUIType::Combo)
                {
                    // JSON 保存的是数值，比如 1
                    // 你自定义的 Combo 使用 int，不需要处理
                }
                else if (p.type == ParamUIType::Int)
                {
                    v = v.toInt();
                }
                else if (p.type == ParamUIType::Double)
                {
                    v = v.toDouble();
                }
                else if (p.type == ParamUIType::Bool)
                {
                    v = v.toBool();
                }
            }

            // ---- 把正确类型的 QVariant 写入参数系统 ----
            setParameter(p.name, v);
        }
    }

    void BaseNodeModel::setParamJson(const QString& name, const QJsonObject& obj)
    {
        QVariantMap map = obj.toVariantMap();
        setParameter(name, map);
    }

    QJsonObject BaseNodeModel::getParamJson(const QString& name) const
    {
        QVariant v = parameterValue(name);
        if (!v.isValid()) return {};

        QVariantMap map = v.toMap();
        return QJsonObject::fromVariantMap(map);
    }

    QVariant BaseNodeModel::parameterValue(const QString& name) const
    {
        return _paramValues.value(name);
    }

    void BaseNodeModel::setParameter(const QString& name, const QVariant& v)
    {
        _paramValues[name] = v;
        applyParameter(name, v);
        if (!_isProcessing)
            update();
        return; // 正在处理时不触发 update
    }

    void BaseNodeModel::applyParameter(const QString& name, const QVariant& v)
    {
        if (!_editors.contains(name))
            return;

        IParamEditor* ed = _editors[name];

        // 阻止信号回流（比如 setValue 触发 valueChanged）
        QSignalBlocker blocker(ed->widget());
        ed->setValue(v);

        // 本地缓存更新
        _paramValues[name] = v;
    }

    void BaseNodeModel::emitOutputs()
    {
        //if (GlobalGraphState::LOADING)
        //    return;
        if (_graphModel && _graphModel->isLoading())
            return;
        for (int i = 0; i < _outputData.size(); i++)
            emit dataUpdated(i);
    }

    BaseGraphModel* BaseNodeModel::findGraphModel(QObject* o)
    {
        while (o)
        {
            if (auto* gm = dynamic_cast<BaseGraphModel*>(o))
                return gm;

            o = o->parent();
        }
        return nullptr;
    }

    std::shared_ptr<QtNodes::NodeData> BaseNodeModel::previewData(int portIndex)
    {
        return outData(portIndex);
    }

}
