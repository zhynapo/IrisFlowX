#pragma once

#include "Analytics.h"

class SolidityFilter : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit SolidityFilter();
    static const NodeDesc desc;

protected:
    void process() override;
};