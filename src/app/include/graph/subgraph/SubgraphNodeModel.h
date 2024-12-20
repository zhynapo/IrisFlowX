// SubgraphNodeModel.h
#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QJsonObject>
#include <QFormLayout>

#include "model/BaseNodeModel.hpp"
#include "SubgraphParam.h"
#include "SubgraphGraphModel.h"
#include "SubgraphScene.h"
#include "SubgraphInputNode.h"
#include "SubgraphOutputNode.h"

using namespace QtNodes;
using namespace Flow;

struct ParamProxy
{
    NodeId internalNode;
    QString name;
    QString label;           // UI Label

    ParamUIType uiType;      // Int/Double/Bool/Combo
    QVariant value;

    QVector<QString> comboNames;
    QVector<int> comboValues;

    QWidget* editor = nullptr;
};

class SubgraphNodeModel : public BaseNodeModel
{
public:
    // 必须保留默认构造（registry->registerModel 需要）
    SubgraphNodeModel() = default;
    // 主程序或子图内部自己创建时可使用这个
    explicit SubgraphNodeModel(std::shared_ptr<BaseNodeRegistry> reg)
    {
        initialize(reg);
    }
    // 手动初始化，必须在创建后调用
    void initialize(std::shared_ptr<BaseNodeRegistry> reg);

    QString name() const override { return "Subgraph"; }
    QString caption() const override { return "Subgraph"; }
    bool captionVisible() const override { return true; }

    void initializeParameters(const std::vector<ExposedParam>& params);
    QWidget* embeddedWidget() override;
    void refreshUI();
    void applyParameterToInternalNode(const ParamProxy& p);

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex index) const override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex index) override;
    std::shared_ptr<NodeData> outData(PortIndex index) override;
    
    void configure(std::vector<NodeId> ins,
                   std::vector<NodeId> outs,
                   std::vector<QString> inTypes,
                   std::vector<QString> outTypes);

    SubgraphGraphModel& graphModel() { return *_graph; }
    SubgraphScene& scene() { return *_scene; }

    // Graph save/load
    QJsonObject save() const override;
    void load(QJsonObject const&) override;

    // Stage 2 load: after main graph is fully created
    void loadSavedSubgraph();
    bool saveSubgraphToFile(QString const& path);
    void loadSubgraphFromJson(QJsonObject const& json);

    void syncPortsFromGraph();

    void connectOutputSignals();
private:
    void reconnectGraphSignals();

private:
    std::shared_ptr<NodeDelegateModelRegistry> _registry;
    std::shared_ptr<SubgraphGraphModel> _graph = nullptr;
    std::shared_ptr<SubgraphScene> _scene;

    QWidget* _embedded = nullptr;      // 只创建一次的 widget
    QFormLayout* _form = nullptr;      // 存储 layout，后续更新内容
    std::unordered_set<NodeId> _connectedOutputs;

    QJsonObject _savedSubgraphJson; // ← delayed load cache

    std::vector<ParamProxy> _params;
    QString _graphName;
    QString _templatePath;

    std::vector<NodeId> _inputNodes;
    std::vector<NodeId> _outputNodes;
    std::vector<QString> _inputTypes;
    std::vector<QString> _outputTypes;
};