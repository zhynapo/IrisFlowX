#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QWidget>
#include <QJsonObject>
#include <QVariant>
#include <QJsonValue>
#include <QJsonArray>
#include <QComboBox>

#include "processing.h"

using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;

struct ColorConvOption {
    const char* name;   // 用于 UI 显示
    int option;           // 对应 cv::COLOR_XXX
};

static const ColorConvOption COLOR_OPTIONS[] = {
    { "BGR -> Gray", cv::COLOR_BGR2GRAY },
    { "BGR -> HSV",  cv::COLOR_BGR2HSV  },
    { "RGB -> Gray", cv::COLOR_RGB2GRAY },
    { "RGB -> HSV",  cv::COLOR_RGB2HSV  },
};
static constexpr int COLOR_OPTIONS_COUNT = sizeof(COLOR_OPTIONS) / sizeof(COLOR_OPTIONS[0]);


class ConvertcolorNode : public BaseNodeModel
{
    Q_OBJECT

public:
    ConvertcolorNode();
    static const NodeDesc desc;

private:
    void process();

private:
    QWidget* _widget = nullptr;
	QComboBox* _combo = nullptr;

    int _cvt_option = cv::COLOR_BGR2GRAY;
    double _sigma = 1.0;
    bool _normalize = false;
    cv::Mat _input;
    cv::Mat _output;
};
