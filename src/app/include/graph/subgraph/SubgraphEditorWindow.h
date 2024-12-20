#pragma once

#include <QMainWindow>
#include <QPointer>
#include "SubgraphNodeModel.h"

namespace QtNodes {
    class GraphicsView;
}

class SubgraphEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SubgraphEditorWindow(SubgraphNodeModel* model,
        QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void buildUi();
    void connectActions();

private:
    QPointer<SubgraphNodeModel> _model;
    QtNodes::GraphicsView* _view = nullptr;
};