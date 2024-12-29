#pragma once

#include "Analytics.h"

class ExtentFilter : public BaseNodeModel
{
    Q_OBJECT

public:
    explicit ExtentFilter();

    static const NodeDesc desc;

protected:
    void process() override;
};