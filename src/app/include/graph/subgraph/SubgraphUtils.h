#pragma once
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include "model/BaseNodeModel.hpp"
#include "SubgraphNodeModel.h"

using namespace QtNodes;
using namespace Flow;


class SubgraphUtils
{
public:
    // 返回在主图创建的新 Subgraph 节点 NodeId
    NodeId createSubgraphFromSelection(
        DataFlowGraphicsScene* mainScene,
        const std::vector<NodeId>& selection);

    bool flattenSubgraph(
        DataFlowGraphModel* mainGraph,
        NodeId subgraphNodeId,
        std::vector<NodeId>& outFlattenedNodes);
};