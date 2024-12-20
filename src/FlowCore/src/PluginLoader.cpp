#include "core/PluginLoader.h"
#include "core/IFlowPlugin.h"
#include "model/BaseNodeRegistry.hpp"

#include <QDir>
#include <QLibrary>
#include <QCoreApplication>

namespace Flow {
    void PluginLoader::loadPlugins(const QString& dirpath,
        FlowRegistryContext& ctx)
    {
        QDir dir(dirpath);
        if (!dir.exists()) {
            // If the path is not found relative to the current working directory,
            // try relative to the application executable directory. This fixes
            // cases where the debugger's working directory differs from the
            // location where plugins were installed by CMake.
            QString alt = QCoreApplication::applicationDirPath() + "/" + dirpath;
            QDir altDir(alt);
            if (altDir.exists())
                dir = altDir;
            else
                return;
        }

        for (auto& file : dir.entryList(QDir::Files))
        {
#if defined(Q_OS_WIN)
            if (!file.endsWith(".dll"))
                continue;
#elif defined(Q_OS_LINUX)
            if (!file.endsWith(".so"))
                continue;
#else
            if (!file.endsWith(".dylib"))
                continue;
#endif

            QString fullpath = dir.absoluteFilePath(file);
            QLibrary lib(fullpath);

            auto reg = (RegisterFlowModuleFn)lib.resolve("register_flow_nodes");
            if (!reg) continue;

            bool ok = reg(&ctx);
            if (!ok) {
                qWarning() << "Node plugin ABI mismatch:" << fullpath;
            }
        }
    }
}