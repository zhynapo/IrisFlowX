#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QtWidgets>
#include <opencv2/opencv.hpp>
#include "Analytics.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;

class ThresholdModel : public BaseNodeModel
{
    Q_OBJECT

public:
    ThresholdModel();
    static const NodeDesc desc;


    QWidget* embeddedWidget() override;

    QJsonObject save() const override;
    void load(const QJsonObject& json) override;

private slots:
    void onAlgorithmChanged(int index);
    void onThresholdChanged(int value);
    void onBlockSizeChanged(int value);
    void onCChanged(int value);

protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _result;

    QWidget* _widget = nullptr;
    QComboBox* _combo;
    QSlider* _slider;
    QLabel* _blockSizeSliderLabel, *_cSliderLabel;
    QSlider* _blockSizeSlider, *_cSlider;

    int _algorithm;   // 0=Binary,1=BinaryInv,2=Trunc,3=ToZero,4=AdaptiveMean,5=AdaptiveGaussian

    enum class ThresholdType {
        Binary,
        BinaryInv,
        Trunc,
        ToZero,
        AdaptiveMean,
        AdaptiveGaussian
    };

    ThresholdType _algo = ThresholdType::Binary;
    int _adaptiveBlockSize = 11;  // Adaptive 参数
    int _adaptiveC = 2;
    int _binaryThreshold = 127;

    int _threshold;   // 全局阈值
};
