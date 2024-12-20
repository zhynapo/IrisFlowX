#include "graph/subgraph/SubgraphUtils.h"

#include "graph/subgraph/SubgraphInputNode.h"
#include "graph/subgraph/SubgraphOutputNode.h"
#include "graph/subgraph/SubgraphGraphModel.h"
#include "graph/subgraph/SubgraphScene.h"

bool SubgraphUtils::flattenSubgraph(
    DataFlowGraphModel* mainGraph,
    NodeId subId,
    std::vector<NodeId>& outFlattened)
{
    auto* subNode = mainGraph->delegateModel<SubgraphNodeModel>(subId);
    if (!subNode) return false;

    auto& sub = subNode->graphModel();   // SubgraphGraphModel

    //----------------------------------------------------------------------
    // Step 0: 分类节点
    //----------------------------------------------------------------------
    std::vector<NodeId> realNodes;
    std::vector<NodeId> inputNodes;
    std::vector<NodeId> outputNodes;

    for (NodeId nid : sub.allNodeIds())
    {
        QString t = sub.nodeData(nid, NodeRole::Type).toString();

        if (t == "SubgraphInput")
            inputNodes.push_back(nid);
        else if (t == "SubgraphOutput")
            outputNodes.push_back(nid);
        else
            realNodes.push_back(nid);
    }

    //----------------------------------------------------------------------
    // Step 1: 将 realNodes 复制到 mainGraph
    // oldRealId → newMainId
    //----------------------------------------------------------------------
    std::unordered_map<NodeId, NodeId> mapReal;

    for (NodeId oldId : realNodes)
    {
        QString type = sub.nodeData(oldId, NodeRole::Type).toString();

        // 在主图中新建一个节点
        NodeId newId = mainGraph->addNode(type);

        mapReal[oldId] = newId;
        outFlattened.push_back(newId);

        // 复制位置
        auto pos = sub.nodeData(oldId, NodeRole::Position).toPointF();
        mainGraph->setNodeData(newId, NodeRole::Position, pos);

        // 复制参数
        // 复制参数：BaseNodeModel 风格
        auto* src = sub.delegateModel<BaseNodeModel>(oldId);
        auto* dst = mainGraph->delegateModel<BaseNodeModel>(newId);
        if (src && dst)
        {
            for (auto const& desc : src->params())
            {
                QVariant v = src->parameterValue(desc.name);
                dst->setParameter(desc.name, v);
            }
        }
    }

    //----------------------------------------------------------------------
    // Step 2: 复制内部连接 (real → real)
    //----------------------------------------------------------------------
    for (NodeId oldId : realNodes)
    {
        auto* mdl = sub.delegateModel<NodeDelegateModel>(oldId);
        int inCount = mdl->nPorts(PortType::In);
        int outCount = mdl->nPorts(PortType::Out);

        // In ports
        for (int ic = 0; ic < inCount; ++ic)
        {
            for (auto const& c : sub.connections(oldId, PortType::In, ic))
            {
                if (mapReal.count(c.outNodeId))
                {
                    ConnectionId nc{
                        mapReal.at(c.outNodeId), c.outPortIndex,
                        mapReal.at(c.inNodeId),  c.inPortIndex
                    };
                    mainGraph->addConnectionDirect(nc);
                    mainGraph->notifyConnectionCreated(nc);
                }
            }
        }

        // Out ports
        for (int oc = 0; oc < outCount; ++oc)
        {
            for (auto const& c : sub.connections(oldId, PortType::Out, oc))
            {
                if (mapReal.count(c.inNodeId))
                {
                    ConnectionId nc{
                        mapReal.at(c.outNodeId), c.outPortIndex,
                        mapReal.at(c.inNodeId),  c.inPortIndex
                    };
                    mainGraph->addConnectionDirect(nc);
                    mainGraph->notifyConnectionCreated(nc);
                }
            }
        }
    }

    //----------------------------------------------------------------------
    // Step 3: 处理外部 → 子图入口
    // 对应 SubgraphInput
    //----------------------------------------------------------------------
    for (NodeId inNode : inputNodes)
    {
        // subgraph 上的入口连接：inNode → internalNode
        for (auto const& c : sub.allConnectionIds(inNode))
        {
            if (c.outNodeId != inNode) continue;

            NodeId internalTarget = c.inNodeId;
            int internalPortIdx = c.inPortIndex;

            // 找主图中对应 subId(inPortIndex) 的外部连接
            for (auto const& ext : mainGraph->allConnectionIds(subId))
            {
                if (ext.inNodeId == subId && ext.inPortIndex == c.outPortIndex)
                {
                    ConnectionId nc{
                        ext.outNodeId, ext.outPortIndex,
                        mapReal.at(internalTarget), internalPortIdx
                    };
                    mainGraph->addConnectionDirect(nc);
                    mainGraph->notifyConnectionCreated(nc);
                }
            }
        }
    }

    //----------------------------------------------------------------------
    // Step 4: 处理子图出口 → 外部
    // 对应 SubgraphOutput
    //----------------------------------------------------------------------
    for (NodeId outNode : outputNodes)
    {
        for (auto const& c : sub.allConnectionIds(outNode))
        {
            if (c.inNodeId != outNode) continue;

            NodeId internalSrc = c.outNodeId;
            int internalPortIdx = c.outPortIndex;

            for (auto const& ext : mainGraph->allConnectionIds(subId))
            {
                if (ext.outNodeId == subId && ext.outPortIndex == c.inPortIndex)
                {
                    ConnectionId nc{
                        mapReal.at(internalSrc), internalPortIdx,
                        ext.inNodeId, ext.inPortIndex
                    };
                    mainGraph->addConnectionDirect(nc);
                    mainGraph->notifyConnectionCreated(nc);
                }
            }
        }
    }

    //----------------------------------------------------------------------
    // Step 5: 删除 subgraph 节点
    // 子图内部 graphModel 会随 model 自动销毁
    //----------------------------------------------------------------------
    mainGraph->deleteNode(subId);

    return true;
}

NodeId SubgraphUtils::createSubgraphFromSelection(
    DataFlowGraphicsScene* mainScene,
    const std::vector<NodeId>& __selection)
{
    if (!mainScene || __selection.empty())
        return InvalidNodeId;


    // 之后所有逻辑都用 realSelection 替代 selection
    //----------------------------------------------------------------------
    // 1. 获取 DataFlowGraphModel
    //----------------------------------------------------------------------
    auto& abs = mainScene->graphModel();   // returns AbstractGraphModel&
    auto* dfGraph = dynamic_cast<DataFlowGraphModel*>(&abs);
    if (!dfGraph)
        return InvalidNodeId;

    //--------------------------------------------------------------------------
    // 1.1 处理 selection：如果包含 Subgraph，需要先 flatten，展开真实节点
    //--------------------------------------------------------------------------
    std::vector<NodeId> realSelection;

    for (NodeId nid : __selection)
    {
        QString type = dfGraph->nodeData(nid, NodeRole::Type).toString();

        if (type == "Subgraph")
        {
            // flatten：得到内部真实节点
            std::vector<NodeId> flattened;
            flattenSubgraph(dfGraph, nid, flattened);

            // 追加展开后的真实节点
            realSelection.insert(realSelection.end(), flattened.begin(), flattened.end());
        }
        else
        {
            realSelection.push_back(nid);
        }
    }

    //----------------------------------------------------------------------
    // 2. 取得 registry（重要：你的版本 registry 在 Graph 上）
    //----------------------------------------------------------------------
    auto* baseGraph = dynamic_cast<BaseGraphModel*>(&abs);
    if (!baseGraph)
        return InvalidNodeId;

    auto registry = baseGraph->registry();   // <-- 这是 BaseNodeRegistry
    //auto registry = dfGraph->dataModelRegistry();

    //----------------------------------------------------------------------
    // 3. 创建 Subgraph 节点
    //----------------------------------------------------------------------
    NodeId subId = dfGraph->addNode("Subgraph");
    auto* subModel = dfGraph->delegateModel<SubgraphNodeModel>(subId);

    // 必须 initialize！否则内部子图无效、UI 不出现、端口为空
    subModel->initialize(registry);

    SubgraphGraphModel& subGraph = subModel->graphModel();
    SubgraphScene& subScene = subModel->scene();

    // old → new nodeId 映射
    std::unordered_map<NodeId, NodeId> old2new;

    //----------------------------------------------------------------------
    // 4. 复制内部节点（不包含 Input/Output Node）
    //----------------------------------------------------------------------
    for (NodeId oldId : realSelection)
    {
        QString type = dfGraph->nodeData(oldId, NodeRole::Type).toString();
        NodeId newId = subGraph.addNode(type);

        old2new[oldId] = newId;

        // 复制位置
        auto pos = dfGraph->nodeData(oldId, NodeRole::Position).toPointF();
        subGraph.setNodeData(newId, NodeRole::Position, pos);
    }

    //----------------------------------------------------------------------
    // 5. 扫描连接并分类
    //----------------------------------------------------------------------
    std::vector<NodeId> inputNodes;
    std::vector<NodeId> outputNodes;
    std::vector<QString> inputTypes;
    std::vector<QString> outputTypes;

    for (NodeId oldId : realSelection)
    {
        auto conns = dfGraph->allConnectionIds(oldId);

        for (auto const& cid : conns)
        {
            bool outInside = old2new.count(cid.outNodeId);
            bool inInside = old2new.count(cid.inNodeId);

            //----------------------------------------------------------------------
            // A. 内部 -> 内部
            //----------------------------------------------------------------------
            if (outInside && inInside)
            {
                subGraph.addConnection({
                    old2new.at(cid.outNodeId),
                    cid.outPortIndex,
                    old2new.at(cid.inNodeId),
                    cid.inPortIndex
                    });
            }

            //----------------------------------------------------------------------
            // B. 外部 -> 内部: 需要 SubgraphInput
            //----------------------------------------------------------------------
            else if (!outInside && inInside)
            {
                NodeId subInId = subGraph.addNode("SubgraphInput");
                auto* mdl = subGraph.delegateModel<SubgraphInputNode>(subInId);

                auto* dst = dfGraph->delegateModel<NodeDelegateModel>(cid.inNodeId);
                QString type = dst->dataType(PortType::In, cid.inPortIndex).id;

                mdl->setType(type);
                inputNodes.push_back(subInId);
                inputTypes.push_back(type);

                subGraph.addConnection({
                    subInId,
                    0,
                    old2new.at(cid.inNodeId),
                    cid.inPortIndex
                    });
            }

            //----------------------------------------------------------------------
            // C. 内部 -> 外部: 需要 SubgraphOutput
            //----------------------------------------------------------------------
            else if (outInside && !inInside)
            {
                NodeId subOutId = subGraph.addNode("SubgraphOutput");
                auto* mdl = subGraph.delegateModel<SubgraphOutputNode>(subOutId);

                auto* src = dfGraph->delegateModel<NodeDelegateModel>(cid.outNodeId);
                QString type = src->dataType(PortType::Out, cid.outPortIndex).id;

                mdl->setType(type);
                outputNodes.push_back(subOutId);
                outputTypes.push_back(type);

                subGraph.addConnection({
                    old2new.at(cid.outNodeId),
                    cid.outPortIndex,
                    subOutId,
                    0
                    });
            }
        }
    }

    //----------------------------------------------------------------------
    // 6. 设置 SubgraphNodeModel 端口配置
    //----------------------------------------------------------------------
    subModel->configure(
        inputNodes,
        outputNodes,
        inputTypes,
        outputTypes);

    //----------------------------------------------------------------------
    // 7. 构建 exposed parameters
    //----------------------------------------------------------------------
    std::vector<ExposedParam> exposeList;

    for (NodeId oldId : realSelection)
    {
        auto* mdl = dfGraph->delegateModel<BaseNodeModel>(oldId);

        for (ParamDesc const& desc : mdl->params())
        {
            //ParamDesc desc = mdl->parameters()[i];
            ExposedParam ep;
            ep.internalNode = old2new.at(oldId);
            ep.name = desc.name;
            ep.label = desc.label;
            ep.uiType = desc.type;
            ep.defaultValue = desc.defaultValue;

            ep.comboNames = desc.comboNames;
            ep.comboValues = desc.comboValues;

            exposeList.push_back(ep);
        }
    }

    subModel->initializeParameters(exposeList);

    // =====================================================================
    // ⭐⭐ 关键步骤：删除选中节点与所有连接（防止重复链路）
    // =====================================================================

    // 1. 收集所有要删除的连接
    std::vector<ConnectionId> connectionsToDelete;

    for (NodeId nid : realSelection)
    {
        for (auto const& cid : dfGraph->allConnectionIds(nid))
        {
            connectionsToDelete.push_back(cid);
        }
    }

    // 2. 去重（可选）
    // 去重 ConnectionId
    std::unordered_set<ConnectionId> uniq;
    std::vector<ConnectionId> filtered;

    for (auto const& c : connectionsToDelete)
    {
        if (uniq.insert(c).second)
            filtered.push_back(c);
    }

    connectionsToDelete.swap(filtered);

    // 3. 删除这些连接
    for (auto const& cid : connectionsToDelete)
    {
        dfGraph->deleteConnection(cid);
    }

    // 4. 删除选中的节点
    for (NodeId nid : realSelection)
    {
        dfGraph->deleteNode(nid);
    }

    // =====================================================================

    return subId;
}