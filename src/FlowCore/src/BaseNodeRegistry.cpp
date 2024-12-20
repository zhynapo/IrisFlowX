#include "model/BaseNodeRegistry.hpp"
#include "model/BaseNodeModel.hpp"
#include "model/BaseGraphModel.hpp"

namespace Flow {
    void BaseNodeRegistry::inject(QtNodes::NodeDelegateModel* mdl)
    {
        qDebug() << "[inject] registry =" << this;
        if (!_currentGraph) return;

        if (auto* bm = dynamic_cast<BaseNodeModel*>(mdl))
            bm->setGraphModel(_currentGraph);
    }

    static BaseNodeRegistry* g_registry = nullptr;
    BaseNodeRegistry* globalNodeRegistry()
    {
        if (!g_registry)
            g_registry = new BaseNodeRegistry();

        return g_registry;
    }
}