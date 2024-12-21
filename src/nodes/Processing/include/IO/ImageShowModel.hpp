#pragma once
#include "processing.h"
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QLabel>

class ImageShowModel : public BaseNodeModel
{
    Q_OBJECT
public:
    ImageShowModel();
    static const NodeDesc desc;

    QWidget* embeddedWidget() override;
    bool resizable() const override { return true; }   // 关键

protected:
    // 子类实现处理逻辑
    void process() override;

private:
    QLabel* _label = nullptr;
    std::shared_ptr<QtNodes::NodeData> _nodeData;

signals:
    void requestPreviewUpdate(const QPixmap&);
};