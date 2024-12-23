// MainWindow.cpp
#include "MainWindow.hpp"

// 在文件顶部添加这些 include（如果还没加）
#include <memory>              // <--- 必须
#include <QMetaObject>

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <QActionGroup>
#include <QSettings>
#include <QStyle>
#include <QtNodes/NodeStyle>
#include <QtNodes/ConnectionStyle>
#include <QStyleFactory>   // 如果你还没加，也建议加
#include <QtGui/QScreen>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QTextEdit>

#include <QMouseEvent>
#include <QGraphicsScene>
#include <QtNodes/internal/NodeGraphicsObject.hpp> // nodeeditor 的图形对象类型

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

//#include "Global.h"
//#include "global_registry.hpp"
#include "Categories.h"
#include "core/IFlowPlugin.h"
#include "core/PluginLoader.h"
#include "Common.hpp"
#include "PixmapData.hpp"
#include "MatNodeData.hpp"

#if 0

#include "IO/ImageLoaderModel.hpp"
#include "IO/ImageShowModel.hpp"

#include "nodes/GaussianModel.hpp"
#include "nodes/PixmapData.hpp" // 确认路径和头名与你工程一致
#include "nodes/AlgorithmDesc.hpp"
#include "nodes/DynamicNodeFactory.hpp"
#include "nodes/GenericNodeModel.hpp"
#include "nodes/DynamicAtlasNode.hpp"

#include "common/RectNodeModel.hpp"

#include "core/imread.hpp"
#include "core/GaussianblurNode.hpp"
#include "core/ConvertcolorNode.hpp"
#include "core/DrawContourNode.hpp"
#include "core/ThresholdModel.hpp"
#include "core/FindContoursNode.hpp"
#include "core/ContoursSelectNode.hpp"
//#include "core/ContourAreaNode.hpp"
//#include "core/ErodeNode.hpp"
//#include "core/ResizeNode.hpp"
#include "core/out/imread2Node.hpp"
#include "basicnodes/test.hpp"
#include "basicnodes/mark.hpp"
#include "basicnodes/ImageCropNodeModel.hpp"
#include "basicnodes/DrawOverlayNode.hpp"
#include "basicnodes/FitEllipseNodeModel.hpp"
#include "basicnodes/BoundingRectNodeModel.hpp"
#endif // 0

#include "graph/GraphSerializer.h"
#include "graph/Subgraph/SubgraphInputNode.h"
#include "graph/Subgraph/SubgraphOutputNode.h"
#include "graph/Subgraph/SubgraphNodeModel.h"
#include "graph/subgraph/SubgraphUtils.h"
#include "graph/subgraph/SubgraphEditorWindow.h"


using namespace QtNodes;

#if 0

static std::shared_ptr<BaseNodeRegistry> makeRegistry()
{
    auto reg = std::shared_ptr<BaseNodeRegistry>(
        globalNodeRegistry(), [](auto*) {}
    );

    // ====== 仅保留 SubgraphNodeModel 特殊注册 ======
    reg->registerModel(
        [reg]() -> std::unique_ptr<NodeDelegateModel> {
            auto m = std::make_unique<SubgraphNodeModel>();
            m->initialize(reg);
            return m;
        },
        "Subgraph"
    );

    return reg;
}
#endif // 0

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{

    // 1. 创建 registry
    _registry = std::make_shared<Flow::BaseNodeRegistry>();

    // 2. 创建 GraphModel，并绑定 registry
    _graphModel = std::make_unique<MainGraphModel>(_registry);
    _registry->setCurrentGraph(_graphModel.get());   // 必须！

    // 3. 加载插件，并注册节点
    Flow::FlowRegistryContext ctx;
    ctx.abi_version = FLOW_ABI_VERSION;
    ctx.register_func = [&](const Flow::NodeModuleInfo& info) {
        _registry->registerModel(info.factory, info.category);
        };
    _registry->registerModel(
        [reg = _registry]() {
            auto m = std::make_unique<SubgraphNodeModel>();
            m->initialize(reg);
            return m;
        },
        "Subgraph"
    );
    Flow::PluginLoader::loadPlugins("plugins/nodes", ctx);

    // 4. Scene + View
    _scene = std::make_unique<DataFlowGraphicsScene>(*_graphModel);
    
#if 0
    Flow::PluginLoader::loadPlugins("plugins/nodes");
    
    _registry = makeRegistry();
    DynamicNodeFactory::registerAtlasNodes(_registry);
    _graphModel = std::make_unique<MainGraphModel>(_registry);
    _scene = std::make_unique<DataFlowGraphicsScene>(*_graphModel);
#endif
    _view = new GraphicsView(_scene.get());
    _view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setCentralWidget(_view);

    _view->setDragMode(QGraphicsView::RubberBandDrag);
    //_scene->setSelectionMode(QtNodes::FlowScene::SelectionMode::MultiSelection);
    // 在 MainWindow 构造函数中
    _view->viewport()->installEventFilter(this);

    // 左侧 Dock（节点菜单将通过rebuildNodesMenu()自动填充）
    _leftDock = new QDockWidget(tr("Node Library"), this);
    auto* dockWidget = new QWidget(_leftDock);
    auto* v = new QVBoxLayout(dockWidget);
    
    // 添加一些通用操作按钮
    auto* btnAddLoader = new QPushButton(tr("Add ImageLoader"));
    auto* btnAddViewer = new QPushButton(tr("Add ImageShow"));
    connect(btnAddLoader, &QPushButton::clicked, this, [this] { addNodeByName("ImageLoaderModel"); });
    connect(btnAddViewer, &QPushButton::clicked, this, [this] { addNodeByName("ImageShowModel"); });
    
    v->addWidget(btnAddLoader);
    v->addWidget(btnAddViewer);
    v->addStretch(1); // 使按钮保持在顶部，菜单项在下面
    
    dockWidget->setLayout(v);
    _leftDock->setWidget(dockWidget);
    addDockWidget(Qt::LeftDockWidgetArea, _leftDock);

    // 右侧 Preview Dock
    _previewDock = new QDockWidget("Preview", this);
    _previewWidget = new ImagePreviewWidget(this);
    _previewDock->setWidget(_previewWidget);
    addDockWidget(Qt::RightDockWidgetArea, _previewDock);

    // 底部 Bottom Dock
    initResultDock();

    createMenus();

    // === Theme Menu ===
    auto themeMenu = menuBar()->addMenu(tr("Theme"));

    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    _actLightTheme = themeMenu->addAction(tr("Light Theme"));
    _actDarkTheme = themeMenu->addAction(tr("Dark Theme"));

    _actLightTheme->setCheckable(true);
    _actDarkTheme->setCheckable(true);

    themeGroup->addAction(_actLightTheme);
    themeGroup->addAction(_actDarkTheme);

    connect(_actLightTheme, &QAction::triggered, this, [this]() { applyTheme(false); });
    connect(_actDarkTheme, &QAction::triggered, this, [this]() { applyTheme(true); });

    QSettings s("MyCompany", "MyApp");
    bool dark = s.value("theme/dark", false).toBool();
    applyTheme(dark);

    resize(1000, 700);
    if (auto* screen = QGuiApplication::primaryScreen()) {
        move(screen->availableGeometry().center() - rect().center());
    }
    setWindowTitle("Data Flow: Resizable Images");

}

void MainWindow::initResultDock()
{
    _resultDock = new QDockWidget("Algorithm Result", this);
    _resultDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    _resultView = new QTextEdit();
    _resultView->setReadOnly(true);
    _resultDock->setWidget(_resultView);
    addDockWidget(Qt::BottomDockWidgetArea, _resultDock);
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    auto actNew = _fileMenu->addAction(tr("&New"));
    auto actOpen = _fileMenu->addAction(tr("&Open..."));
    auto actImport = _fileMenu->addAction(tr("&Import..."));
    auto actSave = _fileMenu->addAction(tr("&Save"));
    auto actSaveAs = _fileMenu->addAction(tr("Save &As..."));
    _fileMenu->addSeparator();
    auto actQuit = _fileMenu->addAction(tr("E&xit"));

    connect(actNew, &QAction::triggered, this, &MainWindow::onNewGraph);
    connect(actOpen, &QAction::triggered, this, &MainWindow::onOpenGraph);
    connect(actImport, &QAction::triggered, this, &MainWindow::onImportSubgraphTemplate);
    connect(actSave, &QAction::triggered, this, &MainWindow::onSaveGraph);
    connect(actSaveAs, &QAction::triggered, this, &MainWindow::onSaveGraphAs);
    connect(actQuit, &QAction::triggered, this, &QWidget::close);

    //_nodesMenu = menuBar()->addMenu(tr("&Nodes"));
    _ProcessingMenu = menuBar()->addMenu(CAT_PROCESSING);
    _AnalyticsMenu = menuBar()->addMenu(CAT_ANALYTICS);
    _ReasoningMenu = menuBar()->addMenu(CAT_REASONING);
    _UtilitiesMenu = menuBar()->addMenu(CAT_UTILITIES);

    rebuildNodesMenu();

    QMenu* viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(_leftDock->toggleViewAction());
    viewMenu->addAction(_previewDock->toggleViewAction());

    // 菜单演示
    auto* menu = menuBar()->addMenu("Subgraph");
    menu->addAction("Create Subgraph", this, &MainWindow::createSubgraphFromSelection);
    menu->addAction("Open Subgraph Editor", this, &MainWindow::openSubgraphEditor);
}

QMenu* ensureMenuPath(QMenu* root, const QString& path)
{
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    QMenu* current = root;

    for (const QString& p : parts)
    {
        // 查找是否已有同名 menu
        QMenu* next = nullptr;
        for (auto* act : current->actions())
        {
            if (auto* m = act->menu(); m && m->title() == p) {
                next = m;
                break;
            }
        }

        // 没有则创建
        if (!next)
            next = current->addMenu(p);

        current = next;
    }

    return current;
}
void MainWindow::rebuildNodesMenu()
{
    // 清空三个一级菜单，但不删除它们
    _ProcessingMenu->clear();
    _AnalyticsMenu->clear();
    _ReasoningMenu->clear();
    _UtilitiesMenu->clear();

    auto creators = _registry->registeredModelCreators();
    auto assoc = _registry->registeredModelsCategoryAssociation();

    for (auto const& kv : creators)
    {
        const QString& modelName = kv.first;

        auto it = assoc.find(modelName);
        if (it == assoc.end())
            continue;

        QString category = it->second;

        if (category.startsWith("__internal__"))
            continue;

        // 分割分类结构
        QStringList parts = category.split("/", Qt::SkipEmptyParts);
        if (parts.isEmpty())
            continue;

        QString top = parts.takeFirst(); // Process / Analysis / Others

        // 找到对应的一级菜单
        QMenu* root = nullptr;
        if (top == CAT_PROCESSING ||  top == "proc")
            root = _ProcessingMenu;
        else if (top == CAT_ANALYTICS || top == "analy")
            root = _AnalyticsMenu;
        else if (top == CAT_REASONING || top == "reason")
            root = _ReasoningMenu;
        else if (top == CAT_UTILITIES || top == "util")
            root = _UtilitiesMenu;
        else
            root = _UtilitiesMenu; // 默认 Utilities
        // 去掉一级分类后剩下的路径
        QString subPath = parts.join("/");

        QMenu* targetMenu = root;

        if (!subPath.isEmpty()) {
            targetMenu = ensureMenuPath(root, subPath);
        }

        QAction* act = targetMenu->addAction(modelName);
        connect(act, &QAction::triggered, this, &MainWindow::onAddNodeByName);
    }
}
void MainWindow::addNodeByName(const QString& name)
{
    _graphModel->addNode(name);
}

void MainWindow::onAddNodeByName()
{
    if (auto* act = qobject_cast<QAction*>(sender()))
        addNodeByName(act->text());
}

void MainWindow::onNewGraph()
{
    _graphModel->load(QJsonObject{});
    _currentFilePath.clear();
}

void MainWindow::onOpenGraph()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open Graph"), {}, tr("Node Graph (*.json)"));
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return;

    //GlobalGraphState::LOADING = true;

    _scene->clearScene();          // 如果你有 NodeGraphicsScene* _scene
    _graphModel->clearGraph();

    const auto doc = QJsonDocument::fromJson(f.readAll());
    GraphSerializer::load(_graphModel.get(), doc.object());

    //GlobalGraphState::LOADING = false;

    _currentFilePath = path;

    // ⭐ 在这里重新触发一次 evaluate，让整个图的数据恢复一致
    // ★ 延迟到事件循环下一轮执行 ★
    QTimer::singleShot(0, this, [this]() {
        _graphModel->evaluateAllNodes();
        });
    // 场景 load 后，模型里已有节点 —— 上面构造函数里那段遍历会为已有节点绑定 dataUpdated（如果尚未绑定）
}

void MainWindow::onImportSubgraphTemplate()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Import Subgraph Template", {}, "Subgraph (*.sub)");
    if (path.isEmpty())
        return;

    QFile f(path);
    f.open(QIODevice::ReadOnly);

    QJsonObject subJson = QJsonDocument::fromJson(f.readAll()).object();
    f.close();

    // ⭐ 1. 在主图中创建一个 SubgraphNodeModel 节点
    NodeId nid = _graphModel->addNode("Subgraph");

    auto* subNode = _graphModel->delegateModel<SubgraphNodeModel>(nid);

    // ⭐ 必须手动初始化
    subNode->initialize(_registry);

    // ⭐ 然后才能加载子图 JSON
    subNode->loadSubgraphFromJson(subJson);

    // ⭐ 再同步端口
    subNode->syncPortsFromGraph();
    
    // ⭐ 4. 放置到鼠标位置
    QPointF pos = _view->mapToScene(_view->rect().center());
    _graphModel->setNodeData(nid, NodeRole::Position, pos);

    // 完成！
}

void MainWindow::onSaveGraph()
{
    if (_currentFilePath.isEmpty()) return onSaveGraphAs();

    QJsonObject root = GraphSerializer::save(_graphModel.get());

    QFile f(_currentFilePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
    }
}

void MainWindow::onSaveGraphAs()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Save Graph As"), {}, tr("Node Graph (*.json)"));
    if (path.isEmpty()) return;

    _currentFilePath = path;
    onSaveGraph();
}

// ----------------- 创建 Subgraph -----------------
void MainWindow::createSubgraphFromSelection()
{
    // 1. Collect selected nodes
    std::vector<NodeId> selectedIds;
    for (auto* item : _scene->selectedItems())
        if (auto* nodeItem = dynamic_cast<NodeGraphicsObject*>(item))
            selectedIds.push_back(nodeItem->nodeId());

    if (selectedIds.empty())
        return;

    // 2. Create Subgraph Node (inside mainGraph)
    NodeId subgraphId =
        _subgraphUtils.createSubgraphFromSelection(_scene.get(), selectedIds);

    if (subgraphId == InvalidNodeId)
        return;

    // 3. Delete original nodes
    for (NodeId nid : selectedIds)
        _graphModel->deleteNode(nid);

    // 4. Position new subgraph node
    QPointF center(0, 0);
    int count = 0;

    for (auto* item : _scene->selectedItems())
        if (auto* nodeItem = dynamic_cast<NodeGraphicsObject*>(item))
            center += nodeItem->pos(), count++;

    if (count > 0)
        center /= count;

    _graphModel->setNodeData(subgraphId, NodeRole::Position, center);

    // 5. Refresh scene
    _scene->clearSelection();
    _scene->update();
}

// ----------------- 打开子图编辑器 -----------------
void MainWindow::openSubgraphEditor()
{
    auto items = _scene->selectedItems();
    if (items.empty()) return;

    for (auto* it : items)
    {
        auto* nodeItem = dynamic_cast<NodeGraphicsObject*>(it);
        if (!nodeItem) continue;

        NodeId nid = nodeItem->nodeId();
        auto* model = _graphModel->delegateModel<NodeDelegateModel>(nid);
        auto* subModel = dynamic_cast<SubgraphNodeModel*>(model);
        if (!subModel) continue;

        // ========== 1. 已经打开的窗口，激活 ==========
        if (_subgraphWindows.contains(subModel))
        {
            auto* win = _subgraphWindows[subModel];
            if (win)
            {
                win->show();
                win->raise();
                win->activateWindow();
                return;
            }
            else
            {
                // pointer expired
                _subgraphWindows.remove(subModel);
            }
        }

        // ========== 2. 创建新窗口 ==========
        auto* win = new SubgraphEditorWindow(subModel, nullptr);
        // no parent → controlled manually
        // no WA_DeleteOnClose

        _subgraphWindows.insert(subModel, win);

        // 当窗口销毁时，从表中移除
        connect(win, &QObject::destroyed, this,
            [this, subModel]()
            {
                _subgraphWindows.remove(subModel);
            });

        // 当节点本身被删除时（GraphModel 删除节点），关闭窗口
        connect(subModel, &QObject::destroyed, this,
            [this, subModel]()
            {
                auto it = _subgraphWindows.find(subModel);
                if (it != _subgraphWindows.end())
                {
                    if (it.value())
                        it.value()->close();
                    _subgraphWindows.remove(subModel);
                }
            });

        win->show();
        win->raise();
        win->activateWindow();
        return;
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == _view->viewport() && ev->type() == QEvent::MouseButtonDblClick) {

        auto* me = static_cast<QMouseEvent*>(ev);
        QPointF scenePos = _view->mapToScene(me->pos());
        auto items = _scene->items(scenePos);

        for (auto* it : items) {
            auto* nodeItem = dynamic_cast<QtNodes::NodeGraphicsObject*>(it);
            if (!nodeItem) continue;

            auto nid = nodeItem->nodeId();
            auto* model = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(nid);
            if (!model) break;
            // 转成 BaseNodeModel
            auto* base = dynamic_cast<BaseNodeModel*>(model);
            if (!base) break;

            // Subgraph 检查
            if (auto* sg = dynamic_cast<SubgraphNodeModel*>(base))
            {
                openSubgraphEditor();
                return true;
            }

            // 处理节点预览
            if (_currentPreviewModel != base) {

                if (_currentModelConnection) {
                    QObject::disconnect(_currentModelConnection);
                    _currentModelConnection = {};
                }

                _currentPreviewModel = base;

                _currentModelConnection = QObject::connect(
                    base,
                    &BaseNodeModel::dataUpdated,
                    this,
                    [this, base](int port) {
                        if (base == _currentPreviewModel)
                            updatePreviewFromModelPort(base, port);
                    },
                    Qt::QueuedConnection
                );

                for (int i = 0; i < base->nPorts(QtNodes::PortType::Out); ++i)
                    updatePreviewFromModelPort(base, i);
            }

            return true;
        }
    }

    return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::updatePreviewFromModelPort(BaseNodeModel* model,
    QtNodes::PortIndex idx)
{
    if (!model) return;

    auto nd = model->previewData(idx);
    if (!nd) return;

    // 由于使用 MatNodeData
    if (auto md = std::dynamic_pointer_cast<MatNodeData>(nd))
    {
        QPixmap pix = MatToQPixmap(md->mat());
        if (!pix.isNull())
            _previewWidget->setPixmap(pix);
    }
}

void MainWindow::updatePreview()
{
    if (!_currentPreviewModel) return;

#if 1

    for (QtNodes::PortIndex i = 0; i < _currentPreviewModel->nPorts(QtNodes::PortType::Out); ++i) {
        QPixmap pix;
        if (auto pd = std::dynamic_pointer_cast<MatNodeData>(_currentPreviewModel->outData(i)))
        {
            pix = MatToQPixmap(pd->mat());
        }

        if (!pix.isNull()) {
            _previewWidget->setPixmap(pix);
        }
    }
#endif // 0

}

void MainWindow::applyTheme(bool dark)
{
    QPalette p;

    if (dark)
    {
        // Dark Palette
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        p.setColor(QPalette::Window, QColor(53,53,53));
        p.setColor(QPalette::WindowText, Qt::white);
        p.setColor(QPalette::Base, QColor(42,42,42));
        p.setColor(QPalette::AlternateBase, QColor(66,66,66));
        p.setColor(QPalette::ToolTipBase, Qt::white);
        p.setColor(QPalette::ToolTipText, Qt::white);
        p.setColor(QPalette::Text, Qt::white);
        p.setColor(QPalette::Button, QColor(53,53,53));
        p.setColor(QPalette::ButtonText, Qt::white);
        p.setColor(QPalette::BrightText, Qt::red);
        p.setColor(QPalette::Highlight, QColor(90,120,200));
        p.setColor(QPalette::HighlightedText, Qt::black);
    }
    else
    {
        // Light Palette
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        p = QApplication::style()->standardPalette();
    }

    QApplication::setPalette(p);

    // ✅ 同时更新 NodeEditor 的绘图背景颜色
    if (_scene) {
        QColor bg = dark ? QColor(40,40,40) : QColor(255,255,255);
        _scene->setBackgroundBrush(bg);
    }

    // ✅ 更新 GraphicsView 的背景
    if (_view) {
        _view->setBackgroundBrush(dark ? QBrush(QColor(36,36,36))
                                       : QBrush(Qt::white));
        _view->viewport()->update();
    }

    // ✅ 菜单项状态保持
    _actDarkTheme->setChecked(dark);
    _actLightTheme->setChecked(!dark);

    // ✅ 保存用户偏好
    QSettings s("MyCompany","MyApp");
    s.setValue("theme/dark", dark);
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    // 关闭所有子图窗口
    for (auto* win : _subgraphWindows)
    {
        if (win)
        {
            win->close();
            win->deleteLater();
        }
    }
    _subgraphWindows.clear();

    QMainWindow::closeEvent(e);
}