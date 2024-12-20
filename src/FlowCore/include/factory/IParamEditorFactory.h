#pragma once
#include "IParamEditor.h"

namespace Flow {
    class IParamEditorFactory
    {
    public:
        virtual ~IParamEditorFactory() = default;
        virtual IParamEditor* create(const ParamDesc& p) = 0;
    };
}