#pragma once
#include <QtNodes/NodeDelegateModelRegistry>
#include "core/FlowCoreExport.h"

namespace Flow {
    class BaseGraphModel;
    class BaseNodeModel;

    class FLOWCORE_API BaseNodeRegistry : public QtNodes::NodeDelegateModelRegistry
    {
    public:
        void setCurrentGraph(BaseGraphModel* gm) {
            qDebug() << "[setCurrentGraph] registry =" << this;
            _currentGraph = gm;
        }

        // 版本1：用户传工厂
        template<typename ModelType>
        void registerModel(RegistryItemCreator creator, QString const& category = "Nodes")
        {
            auto wrapper = [this, creator]() {
                auto mdl = creator();
                inject(mdl.get());
                return mdl;
                };
            NodeDelegateModelRegistry::registerModel<ModelType>(wrapper, category);
        }

        // 版本2：用户不传工厂
        template<typename ModelType>
        void registerModel(QString const& category = "Nodes")
        {
            RegistryItemCreator creator = []() { return std::make_unique<ModelType>(); };

            auto wrapper = [this, creator]() {
                auto mdl = creator();
                inject(mdl.get());
                return mdl;
                };
            NodeDelegateModelRegistry::registerModel<ModelType>(wrapper, category);
        }

        // 版本3：万能推导版本（QtNodes 的模板）
        template<typename ModelCreator>
        void registerModel(ModelCreator&& creator, QString const& category = "Nodes")
        {
            using ModelType = compute_model_type_t<decltype(creator())>;

            auto wrapper = [this, creator]() {
                auto mdl = creator();
                inject(mdl.get());
                return mdl;
                };

            NodeDelegateModelRegistry::registerModel<ModelType>(wrapper, category);
        }

    private:
        void inject(QtNodes::NodeDelegateModel* mdl);

    private:
        BaseGraphModel* _currentGraph = nullptr;
    };

    FLOWCORE_API BaseNodeRegistry* globalNodeRegistry();
}