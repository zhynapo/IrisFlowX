#pragma once
#include <QString>
#include "FlowCoreExport.h"
#include "IFlowPlugin.h"

namespace Flow {

    class FLOWCORE_API PluginLoader
    {
    public:
        static void loadPlugins(const QString& dirpath,
            FlowRegistryContext& ctx);
    };

}