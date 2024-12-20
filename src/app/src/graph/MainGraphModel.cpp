#include "graph/MainGraphModel.h"
#include "graph/subgraph/SubgraphNodeModel.h"
#include <QJsonArray>

QJsonObject MainGraphModel::saveMainGraph() const
{
    QJsonObject graphObj;

    // Save nodes
    QJsonArray nodeArr;
    for (NodeId nid : allNodeIds())
        nodeArr.append(saveNode(nid));
    graphObj["nodes"] = nodeArr;

    // Save connections
    QJsonArray conArr;
    for (NodeId nid : allNodeIds())
        for (auto const& cid : allConnectionIds(nid))
        {
            QJsonObject c;
            c["outNodeId"] = int(cid.outNodeId);
            c["outPortIndex"] = int(cid.outPortIndex);
            c["inNodeId"] = int(cid.inNodeId);
            c["inPortIndex"] = int(cid.inPortIndex);
            conArr.append(c);
        }
    graphObj["connections"] = conArr;

    return graphObj;
}

void MainGraphModel::loadMainGraph(QJsonObject const& graph)
{
    this->load(QJsonObject()); // clears everything

    // Load nodes
    for (auto v : graph["nodes"].toArray())
        loadNode(v.toObject());

    // 2. Load subgraphs BEFORE connections
    for (NodeId nid : this->allNodeIds())
    {
        if (auto* sub = this->delegateModel<SubgraphNodeModel>(nid))
        {
            sub->loadSavedSubgraph();   // ← 关键！！！
        }
    }

    // 3. Load connections AFTER subgraphs are ready
    for (auto v : graph["connections"].toArray())
    {
        QJsonObject cj = v.toObject();
        ConnectionId cid{
            NodeId(cj["outNodeId"].toInt()),
            PortIndex(cj["outPortIndex"].toInt()),
            NodeId(cj["inNodeId"].toInt()),
            PortIndex(cj["inPortIndex"].toInt())
        };
        addConnection(cid);
    }
}

