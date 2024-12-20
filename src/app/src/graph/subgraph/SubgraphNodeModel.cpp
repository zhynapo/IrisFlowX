// SubgraphNodeModel.cpp
#include "graph/subgraph/SubgraphNodeModel.h"
#include <QJsonArray>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>


void SubgraphNodeModel::initialize(std::shared_ptr<BaseNodeRegistry> reg)
{
    _registry = reg;

    reg->registerModel<SubgraphInputNode>("__internal__");
    reg->registerModel<SubgraphOutputNode>("__internal__");

    _graph = std::make_shared<SubgraphGraphModel>(reg);
    _scene = std::make_shared<SubgraphScene>(*_graph);
}

void SubgraphNodeModel::initializeParameters(const std::vector<ExposedParam>& params)
{
    _params.clear();

    for (auto const& p : params)
    {
        ParamProxy proxy;
        proxy.internalNode = p.internalNode;
        proxy.name = p.name;
        proxy.label = p.label;

        proxy.uiType = p.uiType;
        proxy.value = p.defaultValue;

        proxy.comboNames = p.comboNames;
        proxy.comboValues = p.comboValues;

        _params.push_back(proxy);
    }
    // 关键！更新 UI
    //refreshUI();
}

QWidget* SubgraphNodeModel::embeddedWidget()
{
    if (!_embedded)
    {
        _embedded = new QWidget();
        _embedded->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        _form = new QFormLayout(_embedded);
        _form->setContentsMargins(4, 4, 4, 4);     // 控制 padding
        _form->setSpacing(4);
    }

    return _embedded;

}

void SubgraphNodeModel::refreshUI()
{
    // 确保 widget 已生成
    embeddedWidget();

    // 清空旧控件
    while (_form->rowCount() > 0)
    {
        QLayoutItem* item = _form->takeAt(0);
        if (item && item->widget())
            delete item->widget();
        delete item;
    }

    // 为每个参数生成 UI 控件
    for (auto& p : _params)
    {
        QWidget* editor = nullptr;

        switch (p.uiType)
        {
        case ParamUIType::Int: {
            auto* spin = new QSpinBox();
            spin->setValue(p.value.toInt());
            connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                [this, &p](int v) {
                    p.value = v;
                    applyParameterToInternalNode(p);
                });
            editor = spin;
            break;
        }
        case ParamUIType::Double: {
            auto* dspin = new QDoubleSpinBox();
            dspin->setValue(p.value.toDouble());
            connect(dspin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [this, &p](double v) {
                    p.value = v;
                    applyParameterToInternalNode(p);
                });
            editor = dspin;
            break;
        }
        case ParamUIType::Bool: {
            auto* chk = new QCheckBox();
            chk->setChecked(p.value.toBool());
            connect(chk, &QCheckBox::toggled,
                [this, &p](bool v) {
                    p.value = v;
                    applyParameterToInternalNode(p);
                });
            editor = chk;
            break;
        }
        case ParamUIType::Combo:
        {
            auto* combo = new QComboBox();
            combo->addItems(p.comboNames);

            int idx = p.comboValues.indexOf(p.value.toInt());
            combo->setCurrentIndex(idx >= 0 ? idx : 0);

            QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this, &p](int index) {
                    p.value = p.comboValues[index];
                    applyParameterToInternalNode(p);
                });

            editor = combo;
            break;
        }
        }

        p.editor = editor;

        _form->addRow(p.name, editor);
    }

    _embedded->update();
    _embedded->adjustSize();  // ★★★ 关键修复

}

void SubgraphNodeModel::applyParameterToInternalNode(const ParamProxy& p)
{
    auto* mdl = _graph->delegateModel<BaseNodeModel>(p.internalNode);
    if (!mdl) return;

    mdl->setParameter(p.name, p.value);

    // 内部节点参数变了，需要触发计算
    emit mdl->dataUpdated(0);     // 你实际数据端口可能是其它
}

unsigned int SubgraphNodeModel::nPorts(PortType portType) const
{
    if (portType == PortType::In)
        return _inputNodes.size();
    return _outputNodes.size();
}

NodeDataType SubgraphNodeModel::dataType(PortType pt, PortIndex index) const
{
    if (pt == PortType::In)
        return {_inputTypes[index], _inputTypes[index]};
    return {_outputTypes[index], _outputTypes[index]};
}

void SubgraphNodeModel::setInData(std::shared_ptr<NodeData> data, PortIndex index)
{
    if (_graph->isLoading() || !_graph->processingEnabled)
        return;

    NodeId nid = _inputNodes[index];
    //auto* mdl = dynamic_cast<SubgraphInputNode*>(_graph->nodeDelegateModel(nid));
     auto* mdl = _graph->delegateModel<SubgraphInputNode>(nid);
    if (!mdl)
        return;
    if (!_graph->isLoading())
        mdl->setValue(data);      // 会 emit dataUpdated → 子图内部自动执行
    //update();
}

std::shared_ptr<NodeData> SubgraphNodeModel::outData(PortIndex index)
{
    NodeId nid = _outputNodes[index];
    //auto* mdl = dynamic_cast<SubgraphOutputNode*>(_graph->nodeDelegateModel(nid));
    auto* mdl = _graph->delegateModel<SubgraphOutputNode>(nid);
    if (!mdl)
        return nullptr;
    return mdl->value();
}

void SubgraphNodeModel::configure(
    std::vector<NodeId> ins,
    std::vector<NodeId> outs,
    std::vector<QString> inTypes,
    std::vector<QString> outTypes)
{
    _inputNodes = std::move(ins);
    _outputNodes = std::move(outs);
    _inputTypes = std::move(inTypes);
    _outputTypes = std::move(outTypes);

    // ⭐ 子图节点都创建好了，现在必须重新连接 signal-slot
    reconnectGraphSignals();
    // ⭐ 外部输出信号绑定（关键修复）
    connectOutputSignals();
}

void SubgraphNodeModel::reconnectGraphSignals()
{
    return;
    if (!_graph)
        return;

    for (NodeId nid : _graph->allNodeIds())
    {
        auto* mdl = _graph->delegateModel<NodeDelegateModel>(nid);
        if (!mdl)
            continue;

        QObject::connect(
            mdl,
            &NodeDelegateModel::dataUpdated,
            _graph.get(),
            [this, nid](PortIndex port) {
                if (_graph->isLoading() || !_graph->processingEnabled)
                    return;   // 阻止内部自动处理图像
                QMetaObject::invokeMethod(
                    _graph.get(),
                    "onOutPortDataUpdated",
                    Qt::QueuedConnection,
                    Q_ARG(QtNodes::NodeId, nid),
                    Q_ARG(QtNodes::PortIndex, port)
                );
            }
        );
    }
}

void SubgraphNodeModel::connectOutputSignals()
{
    for (int i = 0; i < _outputNodes.size(); ++i)
    {
        NodeId nid = _outputNodes[i];
        if (_connectedOutputs.count(nid))
            continue;   // 已连接，防止重复

        _connectedOutputs.insert(nid);
        auto* mdl = _graph->delegateModel<NodeDelegateModel>(nid);
        if (!mdl) continue;

        QObject::connect(
            mdl, &NodeDelegateModel::dataUpdated,
            this,
            [this, i](PortIndex) {
                emit dataUpdated(i);   //  外部输出更新
            }
        );
    }
}

QJsonObject SubgraphNodeModel::save() const
{
    QJsonObject obj;

    obj["model-name"] = name();

    QJsonArray itA; for (auto& t : _inputTypes)  itA.append(t);
    QJsonArray otA; for (auto& t : _outputTypes) otA.append(t);

    obj["inputTypes"] = itA;
    obj["outputTypes"] = otA;

    // don't save inputNodes/outputNodes here
    // save subgraph JSON
    if (_graph)
        obj["subgraph"] = _graph->saveJson();
    else
        obj["subgraph"] = QJsonObject();

    obj["type"] = "Subgraph";
    obj["name"] = _graphName;
    obj["path"] = _templatePath;

    QJsonObject paramObj;
    for (auto const& p : _params)
    {
        paramObj[p.name] = QJsonValue::fromVariant(p.value);
    }
    obj["params"] = paramObj;

    return obj;
}

void SubgraphNodeModel::load(QJsonObject const& obj)
{
    _graph->beginLoad();

    _inputNodes.clear();
    _outputNodes.clear();

    _inputTypes.clear();
    _outputTypes.clear();

    for (auto v : obj["inputTypes"].toArray())
        _inputTypes.push_back(v.toString());
    for (auto v : obj["outputTypes"].toArray())
        _outputTypes.push_back(v.toString());

    // store subgraph for delayed load
    if (obj.contains("subgraph"))
        _savedSubgraphJson = obj["subgraph"].toObject();


    _graphName = obj["name"].toString();
    _templatePath = obj["path"].toString();

    // 从 JSON 读取参数
    auto paramObj = obj["params"].toObject();

    for (auto& p : _params)
    {
        if (paramObj.contains(p.name))
            p.value = paramObj[p.name].toVariant();
    }

    // 应用到内部节点
    for (auto& p : _params)
        applyParameterToInternalNode(p);

    _graph->endLoad();
}

void SubgraphNodeModel::syncPortsFromGraph()
{
    _graph->beginLoad();

    _inputNodes.clear();
    _outputNodes.clear();
    _inputTypes.clear();
    _outputTypes.clear();

    qDebug() << "All subgraph node IDs:" << _graph->allNodeIds();

    for (NodeId nid : _graph->allNodeIds())
    {
        if (auto* in = _graph->delegateModel<SubgraphInputNode>(nid))
        {
            _inputNodes.push_back(nid);
            auto dt = in->dataType(PortType::Out, 0);
            _inputTypes.push_back(dt.id);
        }
    }

    for (NodeId nid : _graph->allNodeIds())
    {
        if (auto* out = _graph->delegateModel<SubgraphOutputNode>(nid))
        {
            _outputNodes.push_back(nid);
            auto dt = out->dataType(PortType::In, 0);
            _outputTypes.push_back(dt.id);
        }
    }

    // ⭐ 子图节点都创建好了，现在必须重新连接 signal-slot
    reconnectGraphSignals();
    // ⭐ 外部输出信号绑定（关键修复）
    connectOutputSignals();

    _graph->endLoad();
}

// ----------------------------------------------------------
void SubgraphNodeModel::loadSavedSubgraph()
{
    if (_savedSubgraphJson.isEmpty()) return;
    if (!_graph) return;

    _graph->loadJson(_savedSubgraphJson);
    syncPortsFromGraph();
    _savedSubgraphJson = QJsonObject(); // clear after load
}

bool SubgraphNodeModel::saveSubgraphToFile(QString const& path)
{
    if (!_graph) return false;

    QJsonObject subJson = _graph->saveJson();

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;

    f.write(QJsonDocument(subJson).toJson());
    f.close();

    return true;
}

void SubgraphNodeModel::loadSubgraphFromJson(QJsonObject const& json)
{
    if (!_graph) return;
    _graph->beginLoad();

    _graph->clearGraph();
    _graph->loadJson(json);

    _graph->endLoad();
}
