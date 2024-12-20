
#include <QtNodes/GraphicsView>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QFileDialog>
#include <QToolBar>
#include <QAction>
#include <QCloseEvent>
#include <QDebug>

#include "graph/subgraph/SubgraphEditorWindow.h"
#include "graph/subgraph/SubgraphNodeModel.h"
#include "graph/subgraph/SubgraphScene.h"
#include "graph/subgraph/SubgraphGraphModel.h"

SubgraphEditorWindow::SubgraphEditorWindow(SubgraphNodeModel* model,
    QWidget* parent)
    : QMainWindow(parent),
    _model(model)
{
    _model->graphModel().processingEnabled = false;
    setWindowTitle("Subgraph Editor: " + model->caption());
    resize(900, 600);
    buildUi();
    connectActions();
}

void SubgraphEditorWindow::buildUi()
{
    if (!_model)
        return;

    // ★ 使用 GraphicsView 显示子图
    _view = new QtNodes::GraphicsView(&_model->scene());
    setCentralWidget(_view);

    // 工具栏
    auto* tb = addToolBar("Subgraph Tools");
    tb->setMovable(false);

    QAction* actSave = tb->addAction("Save Subgraph");
    actSave->setObjectName("actSave");

    QAction* actRefresh = tb->addAction("Refresh Ports");
    actRefresh->setObjectName("actRefresh");
}

void SubgraphEditorWindow::connectActions()
{
    if (!_model)
        return;

    auto* tb = findChild<QToolBar*>();
    if (!tb)
        return;

    QAction* saveAct = tb->findChild<QAction*>("actSave");
    QAction* refreshAct = tb->findChild<QAction*>("actRefresh");

    if (saveAct)
    {
        connect(saveAct, &QAction::triggered, this, [this]() {
            if (!_model) return;

            // 同步端口（用户编辑完成）
            _model->syncPortsFromGraph();

            QString path = QFileDialog::getSaveFileName(
                this, "Save Subgraph Template", {}, "Subgraph (*.sub)");

            if (!path.isEmpty())
                _model->saveSubgraphToFile(path);
            });
    }

    if (refreshAct)
    {
        connect(refreshAct, &QAction::triggered, this, [this]() {
            if (_model)
            {
                _model->syncPortsFromGraph();
                qDebug() << "[SubgraphEditor] Ports refreshed.";
            }
            });
    }
}

void SubgraphEditorWindow::closeEvent(QCloseEvent* event)
{
    if (_model)
    {
        _model->graphModel().processingEnabled = true;
        _model->syncPortsFromGraph();
        qDebug() << "[SubgraphEditor] Closed → sync ports.";
    }

    QMainWindow::closeEvent(event);
}