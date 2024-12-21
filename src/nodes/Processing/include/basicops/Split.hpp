#pragma once
#include "processing.h"

class Split : public BaseNodeModel
{
    Q_OBJECT
public:
    Split();
    static const NodeDesc desc;
    ~Split() override = default;

    unsigned int nPorts(QtNodes::PortType type) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType type,
                                   QtNodes::PortIndex index) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex portIndex);
    bool portCaptionVisible(QtNodes::PortType type,
                        QtNodes::PortIndex index) const override;
    QString portCaption(QtNodes::PortType type,  QtNodes::PortIndex index) const override;
    
    // ---- Processing ----
protected:
    void process() override;
    void updateChannelCount(std::shared_ptr<QtNodes::NodeData> data);

private:
    cv::Mat _input;
    cv::Mat _output;

    int _thresh = 128;
    int _maxval = 255;
    int _type = 0;

    int _channelCount = 0;
};
