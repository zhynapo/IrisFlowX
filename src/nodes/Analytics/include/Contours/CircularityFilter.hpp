#pragma once

#include "Analytics.h"

class CircularityFilter : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit CircularityFilter();

    static const NodeDesc desc;

protected:
    void process() override;
};