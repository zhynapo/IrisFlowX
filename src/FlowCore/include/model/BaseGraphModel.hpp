
#pragma once

#include <QtNodes/DataFlowGraphModel>
#include "model/BaseNodeRegistry.hpp"
#include "factory/IParamEditorFactory.h"
#include "widgets/ParamEditorFactory.h"


using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeDataType;
using QtNodes::NodeId;

namespace Flow {
    class DefaultParamEditorFactory : public Flow::IParamEditorFactory
    {
    public:
        IParamEditor* create(const ParamDesc& p) override
        {
            return ParamEditorFactory::create(p); // 调用你已有的静态工厂
        }
    };
    class FLOWCORE_API BaseGraphModel : public QtNodes::DataFlowGraphModel
    {
        Q_OBJECT
    public:
        BaseGraphModel(std::shared_ptr<BaseNodeRegistry> reg);
        virtual ~BaseGraphModel();

        //void setLoading(bool b) { loading = b; }
        //bool isLoading() const { return loading; }
        std::shared_ptr<BaseNodeRegistry> registry() const   // <--- 加这个
        {
            return _registry;
        }
        void setParamEditorFactory(IParamEditorFactory* f)
        {
            _editorFactory = f;
        }

        IParamEditorFactory* paramEditorFactory() const
        {
            return _editorFactory;
        }

        bool isLoading() const { return m_loading; }
        void beginLoad() { m_loading = true; }
        void endLoad() { m_loading = false; }

        bool processingEnabled = true;
        bool loading = false;

    signals:
        void graphRestored();

    public slots:
        void evaluateAllNodes();

    private:
        bool m_loading = false;
        std::shared_ptr<BaseNodeRegistry>  _registry;
        IParamEditorFactory* _editorFactory = nullptr;

    };
}