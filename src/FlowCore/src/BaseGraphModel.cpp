
#include "model/BaseGraphModel.hpp"
#include "model/BaseNodeModel.hpp"

namespace Flow {
    BaseGraphModel::BaseGraphModel(std::shared_ptr<BaseNodeRegistry> reg)
        : QtNodes::DataFlowGraphModel(reg), _registry(reg)
    {
        _editorFactory = new DefaultParamEditorFactory();   // ★
        // 加载完 → 重算一次
        connect(this, &BaseGraphModel::graphRestored,
            this, &BaseGraphModel::evaluateAllNodes);
    }
    BaseGraphModel::~BaseGraphModel() = default;

    void BaseGraphModel::evaluateAllNodes()
    {
        std::vector<QtNodes::NodeId> sourceNodes;

        for (QtNodes::NodeId nid : allNodeIds())
        {
            bool hasInputConnection = false;

            // 遍历此节点所有连接
            for (auto const& cid : allConnectionIds(nid))
            {
                if (cid.inNodeId == nid)
                {
                    hasInputConnection = true;
                    break;
                }
            }

            if (!hasInputConnection)
                sourceNodes.push_back(nid);
        }

        // 从源节点开始触发计算
        for (QtNodes::NodeId nid : sourceNodes)
        {
            auto* mdl = delegateModel<QtNodes::NodeDelegateModel>(nid);
            if (!mdl) continue;

            auto* base = dynamic_cast<BaseNodeModel*>(mdl);
            if (!base) continue;

            base->evaluateManually();
        }
    }
}