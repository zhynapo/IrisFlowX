#pragma once
#include <QLabel>
#include "Analytics.h"


class ContourAreaNode : public BaseNodeModel
{
    Q_OBJECT
public:
    ContourAreaNode();
    static const NodeDesc desc;
    
    void process() override;
    QWidget* embeddedWidget() override;

private:
    QLabel* _label = nullptr;
    double _area = 0;
};
