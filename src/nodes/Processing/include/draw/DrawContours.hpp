#pragma once
#include "processing.h"

class DrawContours : public BaseNodeModel
{
    Q_OBJECT
public:
    static const NodeDesc desc;

    DrawContours();
    // ---- Processing ----
protected:
    void process() override;
};