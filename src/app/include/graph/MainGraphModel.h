#pragma once
#include "model/BaseGraphModel.hpp"

using namespace QtNodes;
using namespace Flow;

class MainGraphModel : public BaseGraphModel
{
    Q_OBJECT
public:
    using BaseGraphModel::BaseGraphModel;

    QJsonObject saveMainGraph() const;
    void loadMainGraph(QJsonObject const& json);
    //NodeId addNode(QString const& modelName);
};
