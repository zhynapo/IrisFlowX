#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMenu>
#include <QTextEdit>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "model/BaseNodeRegistry.hpp"

#include "graph/MainGraphModel.h"
#include "graph/subgraph/SubgraphUtils.h"
#include "ImagePreviewWidget.hpp"
#include "graph/subgraph/SubgraphEditorWindow.h"




class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onNewGraph();
    void onOpenGraph();
    void onImportSubgraphTemplate();
    void onSaveGraph();
    void onSaveGraphAs();
    void onAddNodeByName();      // 菜单点击

    void createSubgraphFromSelection();
    void openSubgraphEditor();

private:
    void addNodeByName(const QString& name); // 按钮和菜单共用
    void createMenus();
    void initResultDock();
    void rebuildNodesMenu();
    //theame
    void applyTheme(bool dark);
    void updatePreview();

protected:
    bool eventFilter(QObject* obj, QEvent* event);
    void closeEvent(QCloseEvent* e);

private:
    std::shared_ptr<BaseNodeRegistry> _registry;
    std::unique_ptr<MainGraphModel> _graphModel;
    std::unique_ptr<QtNodes::DataFlowGraphicsScene> _scene;
    SubgraphUtils _subgraphUtils;
    QtNodes::GraphicsView* _view = nullptr;

    QMenu* _fileMenu = nullptr;
    
    QMenu* _ProcessingMenu = nullptr;//     ← 图像 / AI 算子
    QMenu* _AnalyticsMenu = nullptr;      // ← 统计 / 测量 / 分析
    QMenu* _ReasoningMenu = nullptr;      // ← Insight + Decision（解释与判断）
    QMenu* _UtilitiesMenu = nullptr;      // ← 工具

    QAction* _actLightTheme = nullptr;
    QAction* _actDarkTheme = nullptr;

    QDockWidget* _leftDock = nullptr;
    QDockWidget* _previewDock = nullptr;
    ImagePreviewWidget* _previewWidget = nullptr;
    QDockWidget* _resultDock = nullptr;
	QTextEdit* _resultView = nullptr;

    QString _currentFilePath;

    // 用于跟踪当前正在预览的节点/模型
    QtNodes::NodeId _currentPreviewNode = QtNodes::NodeId();
    QtNodes::NodeDelegateModel* _currentPreviewModel = nullptr;
    QtNodes::NodeGraphicsObject* _currentPreviewNodeItem = nullptr;

    QHash<SubgraphNodeModel*, SubgraphEditorWindow*> _subgraphWindows;
    QMetaObject::Connection _previewConnection;

    // 存储连接，以便后续 disconnect()
    QMetaObject::Connection _currentModelConnection;
    void updatePreviewFromModelPort(BaseNodeModel* model, QtNodes::PortIndex idx);

};