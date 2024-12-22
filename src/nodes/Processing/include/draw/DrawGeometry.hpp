#pragma once
#include "processing.h"

class DrawGeometry : public BaseNodeModel
{
    Q_OBJECT
public:
    static const NodeDesc desc;

    DrawGeometry();
    // ---- Processing ----
protected:
    void process() override;
};
