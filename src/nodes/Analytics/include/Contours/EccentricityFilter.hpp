#pragma once
#include "Analytics.h"

class EccentricityFilter : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit EccentricityFilter();

    static const NodeDesc desc;

protected:
    void process() override;
};