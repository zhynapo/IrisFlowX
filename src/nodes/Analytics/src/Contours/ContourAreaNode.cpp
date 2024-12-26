#include <QVBoxLayout>
#include "ContourAreaNode.hpp"
#include "base/ContoursNodeData.hpp"

const NodeDesc ContourAreaNode::desc =
{
    "ContourArea",            // modelName
    "Contour Area",           // displayName
    CAT_ANALY_CONTOURS,                  // category
    // inputs
    { PortDesc::In("contours", NodeType::Contours) },

    // outputs
    { PortDesc::Out("Area", NodeType::Integer) },

    // parameters
    { }
};

ContourAreaNode::ContourAreaNode()
    : BaseNodeModel(desc)
{
}

QWidget* ContourAreaNode::embeddedWidget()
{
    if (_widget) return _widget;   // BaseNodeModel::_widget

    _widget = new QWidget();
    auto* layout = new QVBoxLayout(_widget);

    _label = new QLabel("Area = 0.00");
    _label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _label->setStyleSheet("font-weight: bold;");

    layout->addWidget(_label);
    layout->addStretch();

    return _widget;
}

void ContourAreaNode::process()
{
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    if (!contourData)
    {
        _area = 0;
        setOutputData(0, nullptr);
        if (_label) _label->setText("Area = 0.00");
        return;
    }

    const auto& contours = contourData->value();
    if (contours.empty())
    {
        _area = 0;
        setOutputData(0, nullptr);
        if (_label) _label->setText("Area = 0.00");
        return;
    }

    // Calculate area of the first contour in the list
    const auto& pts = contours[0];
    _area = cv::contourArea(pts);

    // 更新 label
    if (_label)
        _label->setText(QString("Area = %1").arg(_area, 0, 'f', 2));

    // 设置输出
    setOutputData(0, std::make_shared<NumberNodeData>(_area));
}