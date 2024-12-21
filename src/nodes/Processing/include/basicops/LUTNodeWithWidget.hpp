#pragma once
#include "processing.h"

class LUTNodeWithWidget : public BaseNodeModel
{
    Q_OBJECT

public:
    LUTNodeWithWidget();
    static const NodeDesc desc;

#if 0
    QWidget* embeddedWidget() override;

    void saveCurve();
    void loadCurve();
private slots:
    void onCurveChanged();
#endif

    // ---- Processing ----
protected:
    void process() override;

private:
    //QWidget* widget = nullptr;
    //CurveEditorWidget* curve;
    cv::Mat input, output;

};