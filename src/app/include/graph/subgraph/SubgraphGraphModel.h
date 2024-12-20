#pragma once
#include "model/BaseGraphModel.hpp"

using namespace QtNodes;
using namespace Flow;

class SubgraphGraphModel : public BaseGraphModel
{
    Q_OBJECT
public:
    using BaseGraphModel::BaseGraphModel;

    // Save entire subgraph
    QJsonObject saveJson() const;

    // Load subgraph (with ID remapping)
    void loadJson(QJsonObject const &json);

};