#include "graph/GraphSerializer.h"
#include "graph/subgraph/SubgraphNodeModel.h"

// Global flag
QJsonObject GraphSerializer::save(MainGraphModel* model)
{
    QJsonObject root;
    root["graph"] = model->saveMainGraph();  // 子图 JSON 嵌入节点内部
    return root;
}

void GraphSerializer::load(MainGraphModel* model, QJsonObject const& root)
{
    model->loadMainGraph(root["graph"].toObject());

    // second-phase: load subgraphs
    for (NodeId nid : model->allNodeIds())
    {
        if (auto* m = model->delegateModel<SubgraphNodeModel>(nid))
            m->loadSavedSubgraph();     // ← 正确加载子图
    }
}