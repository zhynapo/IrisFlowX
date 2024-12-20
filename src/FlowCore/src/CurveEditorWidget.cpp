#include "widgets/CurveEditorWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QJsonArray>
#include <algorithm>

CurveEditorWidget::CurveEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    // ===== Canvas =====
    _canvas = new CurveCanvas(this);
    root->addWidget(_canvas);

    // ===== Preset Buttons =====
    auto* presetLayout = new QHBoxLayout();
    presetLayout->setContentsMargins(0, 0, 0, 0);

    auto* btnLinear = new QPushButton("Linear");
    auto* btnS = new QPushButton("S");
    auto* btnInv = new QPushButton("Invert");
    auto* btnContrast = new QPushButton("Contrast");
    auto* btnSoft = new QPushButton("Soft");

    presetLayout->addWidget(btnLinear);
    presetLayout->addWidget(btnS);
    presetLayout->addWidget(btnInv);
    presetLayout->addWidget(btnContrast);
    presetLayout->addWidget(btnSoft);
    presetLayout->addStretch();

    root->addLayout(presetLayout);

    // ===== Default curve =====
    setPresetLinear();

    // ===== Preset connections =====
    connect(btnLinear, &QPushButton::clicked, this, [this] { setPresetLinear(); });
    connect(btnS, &QPushButton::clicked, this, [this] { setPresetS(); });
    connect(btnInv, &QPushButton::clicked, this, [this] { setPresetInverse(); });
    connect(btnContrast, &QPushButton::clicked, this, [this] { setPresetContrast(); });
    connect(btnSoft, &QPushButton::clicked, this, [this] { setPresetSoft(); });

    // ===== Canvas → Model (唯一出口) =====
    connect(_canvas, &CurveCanvas::curveChanged,
        this, [this]() {
            _pts = _canvas->points();
            emit curveChanged(saveToJson());
        });
}

QJsonObject CurveEditorWidget::curve() const
{
    return saveToJson();
}

void CurveEditorWidget::setCurve(const QJsonObject& obj)
{
    loadFromJson(obj);
    _canvas->setPoints(_pts);
}

QJsonObject CurveEditorWidget::saveToJson() const
{
    QJsonArray arr;
    for (auto& p : _pts)
    {
        QJsonObject o;
        o["x"] = p.x;
        o["y"] = p.y;
        arr.append(o);
    }

    return QJsonObject{
        { "points", arr }
    };
}

void CurveEditorWidget::loadFromJson(const QJsonObject& obj)
{
    _pts.clear();

    auto arr = obj["points"].toArray();
    for (auto v : arr)
    {
        auto o = v.toObject();
        _pts.push_back({
            float(o["x"].toDouble()),
            float(o["y"].toDouble())
            });
    }

    if (_pts.size() < 2)
        setPresetLinear();
}

//
// ===== Presets =====
//

void CurveEditorWidget::setPresetLinear()
{
    _pts = { {0,0}, {1,1} };
    _canvas->setPoints(_pts);
    emit curveChanged(saveToJson());
}

void CurveEditorWidget::setPresetInverse()
{
    _pts = { {0,1}, {1,0} };
    _canvas->setPoints(_pts);
    emit curveChanged(saveToJson());
}

void CurveEditorWidget::setPresetS()
{
    _pts = { {0,0}, {0.25f,0.15f}, {0.75f,0.85f}, {1,1} };
    _canvas->setPoints(_pts);
    emit curveChanged(saveToJson());
}

void CurveEditorWidget::setPresetContrast()
{
    _pts = { {0,0}, {0.35f,0.25f}, {0.65f,0.75f}, {1,1} };
    _canvas->setPoints(_pts);
    emit curveChanged(saveToJson());
}

void CurveEditorWidget::setPresetSoft()
{
    _pts = { {0,0}, {0.30f,0.20f}, {0.70f,0.80f}, {1,1} };
    _canvas->setPoints(_pts);
    emit curveChanged(saveToJson());
}

//
// ===== LUT =====
//

std::vector<uint8_t> CurveEditorWidget::generateLUT(int size) const
{
    std::vector<uint8_t> lut(size);

    if (_pts.size() < 2)
    {
        for (int i = 0; i < size; ++i)
            lut[i] = uint8_t(i * 255 / (size - 1));
        return lut;
    }

    for (int i = 0; i < size; ++i)
    {
        float x = float(i) / (size - 1);

        int idx = 0;
        while (idx + 1 < _pts.size() && _pts[idx + 1].x < x)
            idx++;

        int i0 = std::max(0, idx - 1);
        int i1 = idx;
        int i2 = std::min(idx + 1, int(_pts.size() - 1));
        int i3 = std::min(idx + 2, int(_pts.size() - 1));

        float t = (x - _pts[i1].x) /
            std::max(1e-5f, _pts[i2].x - _pts[i1].x);

        float y = catmull(
            _pts[i0].y, _pts[i1].y,
            _pts[i2].y, _pts[i3].y, t
        );

        y = std::clamp(y, 0.f, 1.f);
        lut[i] = uint8_t(y * 255);
    }
    return lut;
}

float CurveEditorWidget::catmull(
    float p0, float p1,
    float p2, float p3,
    float t) const
{
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * (
        2.f * p1 +
        (-p0 + p2) * t +
        (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 +
        (-p0 + 3 * p1 - 3 * p2 + p3) * t3
        );
}
