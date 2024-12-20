#pragma once
#include "MainGraphModel.h"

class GraphSerializer
{
public:
    static QJsonObject save(MainGraphModel* model);
    static void load(MainGraphModel* model, QJsonObject const& root);
};