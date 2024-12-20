#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMenu>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onNewGraph();
    void onOpenGraph();
    void onSaveGraph();
    void onSaveGraphAs();
    void onAddNodeByName();      // 菜单点击
private:
    void addNodeByName(const QString& name); // 按钮和菜单共用
    void createMenus();
    void rebuildNodesMenu();

private:
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> _registry;
    std::unique_ptr<QtNodes::DataFlowGraphModel> _graphModel;
    std::unique_ptr<QtNodes::DataFlowGraphicsScene> _scene;
    QtNodes::GraphicsView* _view = nullptr;

    QMenu* _fileMenu = nullptr;
    QMenu* _nodesMenu = nullptr;

    QDockWidget* _leftDock = nullptr;

    QString _currentFilePath;
};