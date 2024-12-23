#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonValue>
#include "ExtractChannelNode.hpp"
#include "Nodetype.hpp"

const NodeDesc ExtractChannelNode::desc =
{
    "ExtractChannel",
    "Extract Channel",
    CAT_PROC_BASIC,

    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("image", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeCombo("channel", "Channel", 
            QVector<QString>{"Channel 0", "Channel 1", "Channel 2", "Channel 3"},
            QVector<int>{0, 1, 2, 3},
            0)
    }
};

ExtractChannelNode::ExtractChannelNode()
    : BaseNodeModel(ExtractChannelNode::desc)
{
}

void ExtractChannelNode::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();

    if (_input.empty())
    {
        setOutputData(0, nullptr);
        return;
    }

    // Get the channel to extract
    int channel = parameterValue("channel").toInt();

    // Check if the image has enough channels
    int numChannels = _input.channels();
    if (channel >= numChannels)
    {
        setOutputData(0, nullptr);
        return;
    }

    // Extract the specified channel
    std::vector<cv::Mat> channels;
    cv::split(_input, channels);

    if (channel < static_cast<int>(channels.size()))
    {
        _output = channels[channel];
    }
    else
    {
        setOutputData(0, nullptr);
        return;
    }

    // Set the output
    setOutputData(0, std::make_shared<MatNodeData>(_output));
}