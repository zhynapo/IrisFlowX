#include "graph/subgraph/SubgraphGraphModel.h"
#include <QJsonArray>

QJsonObject SubgraphGraphModel::saveJson() const
{
    QJsonObject root;
    QJsonObject graphObj;

    // Save nodes
    QJsonArray nodeArr;
    for (NodeId nid : allNodeIds())
    {
        QJsonObject nodeJson = saveNode(nid);

        QJsonObject geo;
        QPointF pos = nodeData(nid, NodeRole::Position).value<QPointF>();
        geo["x"] = pos.x();
        geo["y"] = pos.y();

        QVariant sizeVar = nodeData(nid, NodeRole::Size);
        if (sizeVar.isValid())
        {
            QSize s = sizeVar.toSize();
            geo["w"] = s.width();
            geo["h"] = s.height();
        }

        nodeJson["geometry"] = geo;
        nodeArr.append(nodeJson);
    }
    graphObj["nodes"] = nodeArr;

    // Save connections
    QJsonArray conArr;
    for (NodeId nid : allNodeIds())
        for (auto const& cid : allConnectionIds(nid))
        {
            QJsonObject cj;
            cj["outNodeId"] = int(cid.outNodeId);
            cj["outPortIndex"] = int(cid.outPortIndex);
            cj["inNodeId"] = int(cid.inNodeId);
            cj["inPortIndex"] = int(cid.inPortIndex);
            conArr.append(cj);
        }
    graphObj["connections"] = conArr;

    root["graph"] = graphObj;
    return root;
}

void SubgraphGraphModel::loadJson(QJsonObject const& json)
{
    loading = true;

    this->load(QJsonObject()); // clear

    if (!json.contains("graph")) return;
    QJsonObject graphObj = json["graph"].toObject();

    QHash<int, NodeId> idMap;

    // Load nodes
    for (auto v : graphObj["nodes"].toArray())
    {
        QJsonObject nodeJson = v.toObject();
        int oldId = nodeJson["id"].toInt();

        QString modelName =
            nodeJson["internal-data"].toObject()["model-name"].toString();

        NodeId newId = addNode(modelName);
        idMap[oldId] = newId;

        nodeJson["id"] = int(newId);
        loadNode(nodeJson);

        QJsonObject geo = nodeJson["geometry"].toObject();
        QPointF pos(geo["x"].toDouble(), geo["y"].toDouble());
        setNodeData(newId, NodeRole::Position, QVariant::fromValue(pos));
    }

    // Load connections
    for (auto v : graphObj["connections"].toArray())
    {
        QJsonObject cj = v.toObject();

        ConnectionId cid{
            idMap[cj["outNodeId"].toInt()],
            PortIndex(cj["outPortIndex"].toInt()),
            idMap[cj["inNodeId"].toInt()],
            PortIndex(cj["inPortIndex"].toInt())
        };

        addConnection(cid);
    }

    loading = false;
    emit graphRestored(); // 外部图恢复结束
}
