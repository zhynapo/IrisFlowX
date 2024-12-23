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
#include "PyrDownNode.hpp"
#include "Nodetype.hpp"

const NodeDesc PyrDownNode::desc =
{
    "PyrDown",
    "Pyr Down (Image Reduction)",
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
        ParamDesc::makeCombo("borderType", "Border Type", 
            QVector<QString>{"Default", "Replicate", "Reflect", "Wrap", "Constant"},
            QVector<int>{cv::BORDER_DEFAULT, cv::BORDER_REPLICATE, cv::BORDER_REFLECT, cv::BORDER_WRAP, cv::BORDER_CONSTANT},
            cv::BORDER_DEFAULT)
    }
};

PyrDownNode::PyrDownNode()
    : BaseNodeModel(PyrDownNode::desc)
{
}

void PyrDownNode::process()
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

    // Get parameters
    int borderType = parameterValue("borderType").toInt();

    // Apply pyrDown operation to reduce the image size by half in each dimension
    cv::pyrDown(_input, _output, cv::Size(_input.cols / 2, _input.rows / 2), borderType);

    // Set the output
    setOutputData(0, std::make_shared<MatNodeData>(_output));
}