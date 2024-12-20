// SubgraphScene.h
#pragma once
#include <QtNodes/DataFlowGraphicsScene>
#include "SubgraphGraphModel.h"

using namespace QtNodes;

class SubgraphScene : public DataFlowGraphicsScene
{
public:
    SubgraphScene(SubgraphGraphModel &graph)
        : DataFlowGraphicsScene(graph)
    {}
};