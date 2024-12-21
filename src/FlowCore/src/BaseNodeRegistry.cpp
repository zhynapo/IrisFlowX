#include "model/BaseNodeRegistry.hpp"
#include "model/BaseNodeModel.hpp"
#include "model/BaseGraphModel.hpp"

namespace Flow {
    void BaseNodeRegistry::inject(QtNodes::NodeDelegateModel* mdl)
    {
        //qDebug() << "[inject] registry =" << this;
        if (!_currentGraph) return;

        if (auto* bm = dynamic_cast<BaseNodeModel*>(mdl)) {
            // set graph model
            bm->setGraphModel(_currentGraph);
            // output registered node info (model name and display caption)
            qDebug() << "[inject] registered node:" << bm->name() << ":" << bm->caption();
        }
    }

    static BaseNodeRegistry* g_registry = nullptr;
    BaseNodeRegistry* globalNodeRegistry()
    {
        if (!g_registry)
            g_registry = new BaseNodeRegistry();

        return g_registry;
    }
}