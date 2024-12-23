#include "LUTNodeWithWidget.hpp"
#include <QPushButton>
#include "widgets/CurveEditorWidget.h"
#include <vector>
#include <QJsonArray>

const NodeDesc LUTNodeWithWidget::desc =
{
    "Lut",
    "Lut",
    CAT_PROC_BASIC,  // Category:

    // inputs
    {
        PortDesc::In("mat", NodeType::Mat),
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeCurve("curve", "Tone Curve")
        //ParamDesc::makeDouble("alpha", "alpha", 0.5).range(-1,1),
        //ParamDesc::makeDouble("beta",  "beta",  0.5).range(-1,1)
    }
};

LUTNodeWithWidget::LUTNodeWithWidget()
    : BaseNodeModel(desc)
{
}

#if 0

QWidget* LUTNodeWithWidget::embeddedWidget()
{ 
    if (widget) return widget;

    QWidget* root = new QWidget();
    QVBoxLayout* v = new QVBoxLayout(root);

    curve = new CurveEditorWidget();
    v->addWidget(curve);

    QJsonObject j = getParamJson("curve");
    if (!j.isEmpty() && j.contains("points"))
        curve->loadFromJson(j);
    // 按钮行
    QHBoxLayout* h = new QHBoxLayout();
    QPushButton* linearBtn = new QPushButton("Linear");
    QPushButton* sBtn = new QPushButton("S");
    QPushButton* invBtn = new QPushButton("Inverse");
    QPushButton* contrastBtn = new QPushButton("Contrast");
    QPushButton* softBtn = new QPushButton("Soft");

    h->addWidget(linearBtn);
    h->addWidget(sBtn);
    h->addWidget(invBtn);
    h->addWidget(contrastBtn);
    h->addWidget(softBtn);

    v->addLayout(h);

    // 保存 / 加载
    QHBoxLayout* h2 = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("Save");
    QPushButton* loadBtn = new QPushButton("Load");
    h2->addWidget(saveBtn);
    h2->addWidget(loadBtn);
    v->addLayout(h2);

    // 连接信号
    connect(linearBtn, &QPushButton::clicked, curve, &CurveEditorWidget::setPresetLinear);
    connect(sBtn, &QPushButton::clicked, curve, &CurveEditorWidget::setPresetS);
    connect(invBtn, &QPushButton::clicked, curve, &CurveEditorWidget::setPresetInverse);
    connect(contrastBtn, &QPushButton::clicked, curve, &CurveEditorWidget::setPresetContrast);
    connect(softBtn, &QPushButton::clicked, curve, &CurveEditorWidget::setPresetSoft);

    connect(curve, &CurveEditorWidget::curveChanged, this, &LUTNodeWithWidget::onCurveChanged);

    connect(saveBtn, &QPushButton::clicked, this, &LUTNodeWithWidget::saveCurve);
    connect(loadBtn, &QPushButton::clicked, this, &LUTNodeWithWidget::loadCurve);

    widget = root;
    return widget;
 }

void LUTNodeWithWidget::onCurveChanged()
{
    saveCurve();
    update();
}

void LUTNodeWithWidget::saveCurve()
{
    QJsonObject obj = curve->saveToJson();
    setParamJson("curve", obj);
}

void LUTNodeWithWidget::loadCurve()
{
    QJsonObject obj = getParamJson("curve");
    if (!obj.isEmpty())
        curve->loadFromJson(obj);
}
#endif // 0

float catmull(float p0, float p1, float p2, float p3, float t)
{
    float t2 = t * t, t3 = t2 * t;
    return 0.5f * ((2 * p1)
        + (-p0 + p2) * t
        + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2
        + (-p0 + 3 * p1 - 3 * p2 + p3) * t3);
}

std::vector<uint8_t> generateLUT(std::vector<Pt> &pts) 
{
    std::vector<uint8_t> lut(256, 0);

    if (pts.empty())
    {
        // 1️⃣ 最安全：恒等映射
        for (int i = 0; i < 256; ++i)
            lut[i] = static_cast<uint8_t>(i);
        return lut;
    }

    if (pts.size() == 1)
    {
        std::fill(lut.begin(), lut.end(),
            static_cast<uint8_t>(pts.front().y));
        return lut;
    }

    // ---- 构造扩展控制点（两端各复制两个） ----
    std::vector<Pt> P;
    P.reserve(pts.size() + 4);

    P.push_back(pts.front());
    P.push_back(pts.front());

    for (auto& p : pts)
        P.push_back(p);

    P.push_back(pts.back());
    P.push_back(pts.back());

    int N = P.size(); // >= 6

    // ---- 生成 LUT ----
    for (int i = 0; i < 256; i++)
    {
        float x = i / 255.f;

        int k = 2;
        while (k < N - 3 && x > P[k + 1].x)
            k++;

        const auto& p0 = P[k - 1];
        const auto& p1 = P[k];
        const auto& p2 = P[k + 1];
        const auto& p3 = P[k + 2];

        float denom = (p2.x - p1.x);
        float t = denom > 1e-6f ? (x - p1.x) / denom : 0.0f;
        t = std::clamp(t, 0.f, 1.f);

        float y = catmull(p0.y, p1.y, p2.y, p3.y, t);
        y = std::clamp(y, 0.f, 1.f);

        lut[i] = uint8_t(y * 255.f);
    }
    return lut;
}

void LUTNodeWithWidget::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        // 输入图像不存在，输出置空
        setOutputData(0, nullptr);
        return;
    }
    cv::Mat inputMat = matData->mat();

    //auto lut = curve->generateLUT();
    // 1. 从参数系统取曲线 JSON
    QJsonObject curveJson = getParamJson("curve");

    // 2. 解析 JSON → 点
    std::vector<Pt> pts;
    for (auto v : curveJson["points"].toArray())
    {
        QJsonObject p = v.toObject();
        pts.push_back({ float(p["x"].toDouble()), float(p["y"].toDouble()) });
    }

    // 3. 生成 LUT
    auto lut = generateLUT(pts);


    cv::Mat lutMat(1,256,CV_8UC1,lut.data());
    std::cout << inputMat.type() << std::endl;
    cv::LUT(inputMat, lutMat, output);

    setOutputData(0, std::make_shared<MatNodeData>(output));

}