#include "FolderImageLoaderModel.hpp"

#include "Common.hpp"
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QEvent>

using namespace QtNodes;
using namespace Flow;

const NodeDesc FolderImageLoaderModel::desc = 
{
    "FolderImageLoaderModel",
    "Image Source",
    CAT_PROC_IO,
    {  },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        ParamDesc::makeString("folderpath", "Folder Path", ""),   // 保存文件夹路径
        ParamDesc::makeInt("interval", "Interval (ms)", 1000),    // 保存时间间隔
        ParamDesc::makeBool("isPlaying", "Is Playing", false)     // 保存播放状态
    }
};

FolderImageLoaderModel::FolderImageLoaderModel()
    :BaseNodeModel(FolderImageLoaderModel::desc)
{
    // 不在这里创建QTimer，而是在embeddedWidget中创建，确保在GUI线程中
}

QWidget* FolderImageLoaderModel::embeddedWidget()
{
    if (_widget) return _widget;

    _widget = new QWidget();
    auto* layout = new QVBoxLayout(_widget);

    // 文件夹路径选择区域
    auto* folderLayout = new QHBoxLayout();
    _folderPathEdit = new QLineEdit();
    _folderPathEdit->setPlaceholderText("Select folder containing images...");
    _folderSelectButton = new QPushButton("Browse...");
    
    folderLayout->addWidget(_folderPathEdit);
    folderLayout->addWidget(_folderSelectButton);
    
    layout->addLayout(folderLayout);

    // 图像显示区域
    _imageLabel = new QLabel("No image loaded\nSelect a folder to begin");
    _imageLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _imageLabel->setMinimumSize(200, 200);
    _imageLabel->setMaximumSize(500, 300);
    _imageLabel->setStyleSheet("QLabel { background-color : lightgray; color : darkgray; border: 1px solid gray; }");
    
    QFont f = _imageLabel->font();
    f.setBold(true);
    f.setItalic(true);
    _imageLabel->setFont(f);
    
    layout->addWidget(_imageLabel);

    // 控制按钮区域
    auto* controlLayout = new QHBoxLayout();
    _prevButton = new QPushButton("Previous");
    _playPauseButton = new QPushButton("Play");
    _nextButton = new QPushButton("Next");
    
    // 时间间隔设置
    auto* intervalLayout = new QHBoxLayout();
    intervalLayout->addWidget(new QLabel("Interval (ms):"));
    _intervalSpinBox = new QSpinBox();
    _intervalSpinBox->setRange(10, 10000);  // 100ms to 10s
    _intervalSpinBox->setValue(1000);        // 默认1秒
    intervalLayout->addWidget(_intervalSpinBox);
    
    controlLayout->addWidget(_prevButton);
    controlLayout->addWidget(_playPauseButton);
    controlLayout->addWidget(_nextButton);
    
    layout->addLayout(controlLayout);
    layout->addLayout(intervalLayout);

    // 创建并初始化定时器
    _playTimer = new QTimer(this);
    connect(_playTimer, &QTimer::timeout, this, &FolderImageLoaderModel::onTimerTimeout);

    // 连接信号槽
    connect(_folderSelectButton, &QPushButton::clicked, this, &FolderImageLoaderModel::onFolderSelectClicked);
    connect(_prevButton, &QPushButton::clicked, this, &FolderImageLoaderModel::onPreviousClicked);  // 修复拼写错误
    connect(_nextButton, &QPushButton::clicked, this, &FolderImageLoaderModel::onNextClicked);
    connect(_playPauseButton, &QPushButton::clicked, this, &FolderImageLoaderModel::onPlayPauseClicked);
    connect(_folderPathEdit, &QLineEdit::textChanged, this, &FolderImageLoaderModel::onFolderTextChanged);
    connect(_intervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) { 
                setParameter("interval", value); 
                if (_isPlaying) {
                    _playTimer->setInterval(value);
                }
            });

    // 安装事件过滤器
    _imageLabel->installEventFilter(this);

    updateButtonStates();
    
    // 检查是否已有图像，如果有则显示
    if (!_currentMat.empty()) {
        updateImageDisplay();
    }

    return _widget;
}

bool FolderImageLoaderModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _imageLabel) {
        const int w = _imageLabel->width();
        const int h = _imageLabel->height();

        // 处理标签大小调整事件，重新缩放图像
        if (event->type() == QEvent::Resize) {
            if (!_currentMat.empty()) {
                QPixmap pixmap = MatToQPixmap(_currentMat);
                if (!pixmap.isNull()) {
                    QPixmap scaledPixmap = pixmap.scaled(
                        _imageLabel->size() - QSize(10, 10),
                        Qt::KeepAspectRatio, 
                        Qt::SmoothTransformation
                    );
                    
                    _imageLabel->setPixmap(scaledPixmap);
                    
                    // 保留文本信息
                    if (_currentIndex >= 0 && _currentIndex < static_cast<int>(_imagePaths.size())) {
                        QFileInfo fileInfo(_imagePaths[_currentIndex]);
                        QString text = QString("%1\n(%2/%3)")
                            .arg(fileInfo.fileName())
                            .arg(_currentIndex + 1)
                            .arg(_imagePaths.size());
                        _imageLabel->setText(text);
                    }
                }
            }
        }
        // 处理右键点击事件，显示上下文菜单
        else if (event->type() == QEvent::ContextMenu) {
            QMenu menu;
            QAction* actSelectFolder = menu.addAction(tr("Select Folder..."));
            QAction* actClear = !_currentMat.empty() ? menu.addAction(tr("Clear")) : nullptr;

            QAction* chosen = menu.exec(QCursor::pos());
            if (chosen == actSelectFolder) {
                onFolderSelectClicked();
                return true;
            } else if (chosen == actClear) {
                _currentMat = cv::Mat();
                _imageLabel->setPixmap(QPixmap());
                _imageLabel->setText("No image loaded\nSelect a folder to begin");
                Q_EMIT dataUpdated(0);
                return true;
            }
        }
    }
    return BaseNodeModel::eventFilter(object, event);
}

void FolderImageLoaderModel::onFolderSelectClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        _widget,
        tr("Select Image Folder"),
        QDir::homePath()
    );

    if (!folderPath.isEmpty()) {
        _folderPathEdit->setText(folderPath);
        setParameter("folderpath", folderPath);
        loadImagesFromFolder();
        if (!_imagePaths.empty()) {
            setCurrentIndex(0);
        }
    }
}

void FolderImageLoaderModel::onPreviousClicked()
{
    if (_currentIndex > 0) {
        setCurrentIndex(_currentIndex - 1);
    }
}

void FolderImageLoaderModel::onNextClicked()
{
    if (_currentIndex < static_cast<int>(_imagePaths.size()) - 1) {
        setCurrentIndex(_currentIndex + 1);
    }
}

void FolderImageLoaderModel::onPlayPauseClicked()
{
    if (_imagePaths.empty()) {
        return;
    }
    
    if (_isPlaying) {
        // 停止播放
        stopTimer();
        _playPauseButton->setText("Play");
        setParameter("isPlaying", false);
    } else {
        // 开始播放
        startTimer();
        _playPauseButton->setText("Pause");
        setParameter("isPlaying", true);
    }
    
    _isPlaying = !_isPlaying;
    updateButtonStates();
}

void FolderImageLoaderModel::onTimerTimeout()
{
    if (_currentIndex < static_cast<int>(_imagePaths.size()) - 1) {
        // 还有下一张图片
        setCurrentIndex(_currentIndex + 1);
    } else {
        // 回到第一张（循环播放）
        setCurrentIndex(0);
    }
}

void FolderImageLoaderModel::onFolderTextChanged(const QString& text)
{
    if (!text.isEmpty()) {
        setParameter("folderpath", text);
        loadImagesFromFolder();
        if (!_imagePaths.empty() && _currentIndex == -1) {
            setCurrentIndex(0);
        }
    }
}

void FolderImageLoaderModel::loadImagesFromFolder()
{
    _imagePaths.clear();
    _currentIndex = -1;

    QString folderPath = _folderPathEdit->text();
    if (folderPath.isEmpty()) return;

    QDir dir(folderPath);
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.tiff" << "*.tif";
    
    QStringList imageFiles = dir.entryList(filters, QDir::Files, QDir::Name);
    
    for (const QString& file : imageFiles) {
        _imagePaths.push_back(dir.filePath(file));
    }
    
    updateButtonStates();
}

void FolderImageLoaderModel::setCurrentIndex(int index)
{
    if (index < 0 || index >= static_cast<int>(_imagePaths.size())) {
        return;
    }

    _currentIndex = index;
    QString imagePath = _imagePaths[_currentIndex];

    // 加载图像
    _currentMat = cv::imread(imagePath.toStdString(), cv::IMREAD_UNCHANGED);
    
    if (!_currentMat.empty()) {
        // 调用process来更新UI显示
        process();
        Q_EMIT dataUpdated(0);
    }
}

void FolderImageLoaderModel::updateImageDisplay()
{
    if (_currentMat.empty()) {
        _imageLabel->setText("No image loaded\nSelect a folder to begin");
        _imageLabel->setPixmap(QPixmap()); // 清除任何现有的pixmap
        return;
    }

    // 显示当前图像
    QPixmap pixmap = MatToQPixmap(_currentMat);
    if (!pixmap.isNull()) {
        // 调整图像大小以适应标签，同时保持宽高比
        QPixmap scaledPixmap = pixmap.scaled(
            _imageLabel->size() - QSize(10, 10), // 留一点边距
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation
        );
        
        // 设置图像
        _imageLabel->setPixmap(scaledPixmap);
        
        // 显示当前图像名称和索引
        if (_currentIndex >= 0 && _currentIndex < static_cast<int>(_imagePaths.size())) {
            QFileInfo fileInfo(_imagePaths[_currentIndex]);
            QString text = QString("%1\n(%2/%3)")
                .arg(fileInfo.fileName())
                .arg(_currentIndex + 1)
                .arg(_imagePaths.size());
            _imageLabel->setText(text);
        }
    } else {
        _imageLabel->setText("Failed to load image");
        _imageLabel->setPixmap(QPixmap());
    }
    
    updateButtonStates();
}

void FolderImageLoaderModel::updateButtonStates()
{
    if (_prevButton) {
        _prevButton->setEnabled(_currentIndex > 0 && !_isPlaying);
    }
    if (_nextButton) {
        _nextButton->setEnabled(_currentIndex < static_cast<int>(_imagePaths.size()) - 1 && !_imagePaths.empty() && !_isPlaying);
    }
    if (_playPauseButton) {
        _playPauseButton->setText(_isPlaying ? "Pause" : "Play");
    }
    if (_intervalSpinBox) {
        _intervalSpinBox->setEnabled(!_isPlaying);
    }
}

void FolderImageLoaderModel::startTimer()
{
    if (_playTimer && !_playTimer->isActive()) {
        int interval = parameterValue("interval").toInt();
        _playTimer->setInterval(interval);
        _playTimer->start();
    }
}

void FolderImageLoaderModel::stopTimer()
{
    if (_playTimer && _playTimer->isActive()) {
        _playTimer->stop();
    }
}

// 重写process方法，更新图像显示
void FolderImageLoaderModel::process()
{
    if (!_currentMat.empty()) {
        // 调整图像大小以适应标签，同时保持宽高比
        QPixmap pixmap = MatToQPixmap(_currentMat);
        if (!pixmap.isNull()) {
            QPixmap scaledPixmap = pixmap.scaled(
                _imageLabel->size() - QSize(10, 10), // 留一点边距
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation
            );
            
            _imageLabel->setPixmap(scaledPixmap);
            qDebug() << "_imageLabel size =" << _imageLabel->size();
            qDebug() << "scaledPixmap isNull?" << scaledPixmap.isNull();

            // 显示当前图像名称和索引
            if (_currentIndex >= 0 && _currentIndex < static_cast<int>(_imagePaths.size())) {
                QFileInfo fileInfo(_imagePaths[_currentIndex]);
                QString text = QString("%1\n(%2/%3)")
                    .arg(fileInfo.fileName())
                    .arg(_currentIndex + 1)
                    .arg(_imagePaths.size());
                //_imageLabel->setText(text);
            }
        }
    }
}

void FolderImageLoaderModel::setInData(std::shared_ptr<NodeData> data, QtNodes::PortIndex portIndex)
{
    // 重写此方法以处理输入，虽然我们的节点没有输入，但为了完整性保留
    BaseNodeModel::setInData(data, portIndex);
}

std::shared_ptr<NodeData> FolderImageLoaderModel::outData(PortIndex)
{
    if (_currentMat.empty()) return nullptr;
    return std::make_shared<MatNodeData>(_currentMat);
}

void FolderImageLoaderModel::load(QJsonObject const& obj)
{
    BaseNodeModel::load(obj);   // 调用基类处理参数恢复

    QString path = parameterValue("folderpath").toString();
    int interval = parameterValue("interval").toInt();
    bool isPlaying = parameterValue("isPlaying").toBool();

    if (!path.isEmpty()) {
        _folderPathEdit->setText(path);
        loadImagesFromFolder();
        if (!_imagePaths.empty()) {
            // 检查是否当前路径就是正在显示的图像路径
            if (_currentIndex >= 0 && _currentIndex < static_cast<int>(_imagePaths.size())) {
                // 重新加载当前图像
                QString imagePath = _imagePaths[_currentIndex];
                _currentMat = cv::imread(imagePath.toStdString(), cv::IMREAD_UNCHANGED);
                if (!_currentMat.empty()) {
                    process(); // 使用process方法更新UI
                }
            } else {
                setCurrentIndex(0);
            }
        }
    }
    
    if (_intervalSpinBox) {
        _intervalSpinBox->setValue(interval);
    }
    
    if (isPlaying && !_imagePaths.empty()) {
        _isPlaying = true;
        onPlayPauseClicked(); // 启动播放
    }
}