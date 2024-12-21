
#include "DrawContourNode.hpp"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>

const NodeDesc DrawContourNode::desc = {
    "Draw Contour",
    "DrawContourNode",
    CAT_PROC_DRAW,
    //------ 输入端口 ------
    {
        PortDesc::In("mat",  NodeType::Mat),     // 输入图像
        PortDesc::In("points", NodeType::Points)   // 输入轮廓点
    },
    { PortDesc::Out("mat", NodeType::Mat) },
    {
        ParamDesc::makeColor("color", "Color", QColor(255,0,0)),
        ParamDesc::makeInt("thickness", "Thickness", 2).setRange(1, 20)
    }
};

DrawContourNode::DrawContourNode()
    : BaseNodeModel(DrawContourNode::desc)
{
}

#if 0
QWidget* DrawContourNode::embeddedWidget()
{

    // 若已创建则直接返回
    if (_widget) return _widget;

    // 你的自定义 widget
    _widget = new QWidget();
    auto layout = new QFormLayout(_widget);

    auto& p = params();  // your param list
    // ---- Color Picker ----
    _colorPicker = new ColorPickerWidget();
    _colorPicker->setColor(p[0].defaultValue.value<QColor>());

    connect(_colorPicker, &ColorPickerWidget::colorChanged,
        this, [this](const QColor& c) {
            setParameter("color", c);
            update();
        });

    layout->addRow("Color:", _colorPicker);

    // ---- Thickness ----
    QSpinBox* _thicknessSpin = new QSpinBox();
    _thicknessSpin->setRange(p[1].minValue, p[1].maxValue);
    _thicknessSpin->setValue(p[1].defaultValue.toInt());

    connect(_thicknessSpin, qOverload<int>(&QSpinBox::valueChanged),
        this, [this](int v) {
            setParameter("thickness", v);
            update();
        });

    layout->addRow("Thickness:", _thicknessSpin);
    return _widget;
}
#endif

void DrawContourNode::process()
{
    //----- 1. 获取图像输入 -----
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat inputMat = matData->mat();


    //----- 2. 获取点集输入 -----
    auto ptsData = std::dynamic_pointer_cast<PointsNodeData>(_getInput(1));
    if (!ptsData)
    {
        setOutputData(0, nullptr);
        return;
    }
    const std::vector<cv::Point>& points = ptsData->value();

    if (points.empty())
    {
        setOutputData(0, nullptr);
        return;
    }


    //----- 3. 克隆输入图像 -----
    cv::Mat output = inputMat.clone();


    //----- 4. 获取参数，例如颜色 -----
    QColor color = parameterValue("color").value<QColor>();
    int thickness = parameterValue("thickness").toInt();

    cv::Scalar cvColor(color.blue(), color.green(), color.red());   // BGR


    //----- 5. 绘制轮廓 -----
    std::vector<std::vector<cv::Point>> contourList;
    contourList.push_back(points);

    cv::drawContours(output, contourList, 0, cvColor, thickness);


    //----- 6. 设置输出 -----
    setOutputData(0, std::make_shared<MatNodeData>(output));
}
