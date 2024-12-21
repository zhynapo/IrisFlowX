#pragma once 
#include <iostream> 
#include <QtCore/QObject> 
#include <QtWidgets/QLabel> 
#include <QtNodes/NodeDelegateModel> 
#include <QtNodes/NodeDelegateModelRegistry> 

#include "processing.h"

using QtNodes::NodeData; 
using QtNodes::NodeDataType; 
using QtNodes::NodeDelegateModel; 

class ImageLoaderModel : public BaseNodeModel {
    Q_OBJECT
public:
    ImageLoaderModel();
    static const NodeDesc desc;

public:
    std::shared_ptr<NodeData> outData(PortIndex const port) override;
    bool resizable() const override { return true; }
    // UI 生成
    QWidget* embeddedWidget() override;
    void load(QJsonObject const& obj) override;
protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    
private:
    QLabel *_label = nullptr;
    cv::Mat _mat;

};