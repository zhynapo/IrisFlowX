#pragma once
#include "processing.h"

class AddWeighted : public BaseNodeModel
{
    Q_OBJECT
public:
    AddWeighted();
    static const NodeDesc desc;
    ~AddWeighted() override = default;

    // ---- Processing ----
protected:
    void process() override;

private:
    cv::Mat _input;
    cv::Mat _output;

    int _ksize = 5;
    double _sigma = 1.2;
    bool _enable = true;
    QString _text = QString("hello");
    int _mode = 1;
};
