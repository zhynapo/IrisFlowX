#pragma once
#include <QWidget>
#include <QJsonObject>
#include <vector>
#include "widgets/CurveCanvas.h"

class CurveEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CurveEditorWidget(QWidget* parent = nullptr);

    QJsonObject curve() const;
    void setCurve(const QJsonObject& obj);

    std::vector<uint8_t> generateLUT(int size = 256) const;

signals:
    void curveChanged(const QJsonObject& obj);

private:
    // JSON
    QJsonObject saveToJson() const;
    void loadFromJson(const QJsonObject& obj);

    // Presets
    void setPresetLinear();
    void setPresetS();
    void setPresetInverse();
    void setPresetContrast();
    void setPresetSoft();

    float catmull(float p0, float p1, float p2, float p3, float t) const;

private:
    CurveCanvas* _canvas = nullptr;
    std::vector<Pt> _pts;
};
