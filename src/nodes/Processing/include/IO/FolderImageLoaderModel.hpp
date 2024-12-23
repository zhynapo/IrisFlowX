#pragma once
#include <iostream>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "processing.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;

class FolderImageLoaderModel : public BaseNodeModel {
    Q_OBJECT
public:
    FolderImageLoaderModel();
    static const NodeDesc desc;

public:
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    bool resizable() const override { return true; }
    // UI 生成
    QWidget* embeddedWidget() override;
    void load(QJsonObject const& obj) override;
    void setInData(std::shared_ptr<NodeData> data,  QtNodes::PortIndex portIndex) override;

protected:
    void process() override;
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void onPreviousClicked();
    void onNextClicked();
    void onFolderSelectClicked();
    void onFolderTextChanged(const QString& text);
    void onPlayPauseClicked();
    void onTimerTimeout();

private:
    void updateImageDisplay();
    void loadImagesFromFolder();
    void updateButtonStates();
    void setCurrentIndex(int index);
    void startTimer();
    void stopTimer();

private:
    QWidget *_widget = nullptr;
    QLabel *_imageLabel = nullptr;
    QPushButton *_prevButton = nullptr;
    QPushButton *_nextButton = nullptr;
    QPushButton *_folderSelectButton = nullptr;
    QPushButton *_playPauseButton = nullptr;
    QLineEdit *_folderPathEdit = nullptr;
    QSpinBox *_intervalSpinBox = nullptr;
    
    std::vector<QString> _imagePaths;
    int _currentIndex = -1;
    cv::Mat _currentMat;
    
    QTimer *_playTimer = nullptr;
    bool _isPlaying = false;
};