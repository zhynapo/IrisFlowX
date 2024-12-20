#include "MainWindow.hpp"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QtGui/QScreen>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>

// 节点
#include "nodes/ImageLoaderModel.hpp"
#include "nodes/ImageShowModel.hpp"
#include "nodes/GaussianModel.hpp"

using namespace QtNodes;

static std::shared_ptr<NodeDelegateModelRegistry> makeRegistry()
{
    auto reg = std::make_shared<NodeDelegateModelRegistry>();
    reg->registerModel<ImageLoaderModel>("IO");
    reg->registerModel<GaussianModel>("Image");
    reg->registerModel<ImageShowModel>("IO");
    return reg;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    _registry = makeRegistry();
    _graphModel = std::make_unique<DataFlowGraphModel>(_registry);
    _scene = std::make_unique<DataFlowGraphicsScene>(*_graphModel);

    _view = new GraphicsView(_scene.get());
    _view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setCentralWidget(_view);

    // 左侧 Dock
    _leftDock = new QDockWidget(tr("Settings"), this);
    auto* dockWidget = new QWidget(_leftDock);
    auto* v = new QVBoxLayout(dockWidget);
    auto* btnAddLoader  = new QPushButton(tr("Add ImageLoader"));
    auto* btnAddGaussian= new QPushButton(tr("Add Gaussian"));
    auto* btnAddViewer  = new QPushButton(tr("Add ImageShow"));
    v->addWidget(btnAddLoader);
    v->addWidget(btnAddGaussian);
    v->addWidget(btnAddViewer);
    v->addStretch(1);
    dockWidget->setLayout(v);
    _leftDock->setWidget(dockWidget);
    addDockWidget(Qt::LeftDockWidgetArea, _leftDock);

    connect(btnAddLoader,   &QPushButton::clicked, this, [this]{ addNodeByName("ImageLoaderModel"); });
    connect(btnAddGaussian, &QPushButton::clicked, this, [this]{ addNodeByName("GaussianModel"); });
    connect(btnAddViewer,   &QPushButton::clicked, this, [this]{ addNodeByName("ImageShowModel"); });

    createMenus();

    resize(1000, 700);
    move(QApplication::primaryScreen()->availableGeometry().center() - rect().center());
    setWindowTitle("Data Flow: Resizable Images");
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    auto actNew    = _fileMenu->addAction(tr("&New"));
    auto actOpen   = _fileMenu->addAction(tr("&Open..."));
    auto actSave   = _fileMenu->addAction(tr("&Save"));
    auto actSaveAs = _fileMenu->addAction(tr("Save &As..."));
    _fileMenu->addSeparator();
    auto actQuit   = _fileMenu->addAction(tr("E&xit"));

    connect(actNew,    &QAction::triggered, this, &MainWindow::onNewGraph);
    connect(actOpen,   &QAction::triggered, this, &MainWindow::onOpenGraph);
    connect(actSave,   &QAction::triggered, this, &MainWindow::onSaveGraph);
    connect(actSaveAs, &QAction::triggered, this, &MainWindow::onSaveGraphAs);
    connect(actQuit,   &QAction::triggered, this, &QWidget::close);

    _nodesMenu = menuBar()->addMenu(tr("&Nodes"));
    rebuildNodesMenu();
}

void MainWindow::rebuildNodesMenu()
{
    _nodesMenu->clear();
    auto cats = _registry->categories();
    auto assoc = _registry->registeredModelsCategoryAssociation();
    auto creators = _registry->registeredModelCreators();

    if (!cats.empty()) {
        for (auto const& cat : cats) {
            auto* sub = _nodesMenu->addMenu(cat);
            for (auto const& kv : creators) {
                const QString& name = kv.first;
                auto it = assoc.find(name);
                if (it != assoc.end() && it->second == cat) {
                    auto* act = sub->addAction(name);
                    connect(act, &QAction::triggered, this, &MainWindow::onAddNodeByName);
                }
            }
        }
    } else {
        for (auto const& kv : creators) {
            auto* act = _nodesMenu->addAction(kv.first);
            connect(act, &QAction::triggered, this, &MainWindow::onAddNodeByName);
        }
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
    // 清空：加载一个空 JSON
    _graphModel->load(QJsonObject{});
    _currentFilePath.clear();
}

void MainWindow::onOpenGraph()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Open Graph"), {}, tr("Node Graph (*.json)"));
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    _graphModel->load(doc.object());
    _currentFilePath = path;
}

void MainWindow::onSaveGraph()
{
    if (_currentFilePath.isEmpty()) return onSaveGraphAs();

    const auto obj = _graphModel->save();
    QFile f(_currentFilePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
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