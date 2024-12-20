#pragma once
#include "model/BaseNodeModel.hpp"
#include "base/MatNodeData.hpp"
#include <opencv2/opencv.hpp>

using namespace Flow;

class LoadImage : public BaseNodeModel
{
    Q_OBJECT
public:
    LoadImage();
    static const NodeDesc desc;
    ~LoadImage() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    QString _path = QString("");
};
