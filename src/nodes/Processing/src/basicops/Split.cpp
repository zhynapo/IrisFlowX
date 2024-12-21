#include "Split.hpp"
#include "Nodetype.hpp"

const NodeDesc Split::desc =
{
    "Split",
    "Split",
    CAT_PROC_BASIC,
    // inputs
    {
        PortDesc::In("image", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("c0", NodeType::Mat),
        PortDesc::Out("c1", NodeType::Mat),
        PortDesc::Out("c2", NodeType::Mat),
        PortDesc::Out("c3", NodeType::Mat)
    },

    // parameters
    {
    }
};

Split::Split()
    : BaseNodeModel(desc)
{
}

void Split::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    _input = matData->mat();


    std::vector<cv::Mat> ch;
    cv::split(_input, ch);
    int C = ch.size();

    // 清空当前输出
    for (int i = 0; i < 4; i++) {
        setOutputData(i, nullptr);
    }

    // 输出实际通道
    for (int i = 0; i < C; i++) {
        setOutputData(i, std::make_shared<MatNodeData>(ch[i]));
    }

    qDebug() << "Split input channels:" << _input.channels();

}

unsigned int Split::nPorts(QtNodes::PortType type) const
{
    if (type == QtNodes::PortType::Out)
        return std::max(1, _channelCount);  // 至少保留1个端口
    return 1;
}

QtNodes::NodeDataType Split::dataType(QtNodes::PortType type,
                                      QtNodes::PortIndex index) const
{
    return { "mat", "Mat" };
}

QString Split::portCaption(QtNodes::PortType type,
                           QtNodes::PortIndex index) const
{
    if (type == QtNodes::PortType::Out)
        return QString("ch%1").arg(index);
    return "";
}
void Split::updateChannelCount(std::shared_ptr<QtNodes::NodeData> data)
{
    // data 是 MatData（假设你用的是这种 wrapper）
    auto matData = std::dynamic_pointer_cast<MatNodeData>(data);
    if (!matData) {
        _channelCount = 0;
        return;
    }

    const cv::Mat &img = matData->mat();
    if (img.empty()) {
        _channelCount = 0;
        return;
    }

    _channelCount = img.channels();
}

void Split::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex portIndex)
{
    // 先调用父类，确保输入数据被记录、update() 被触发
    BaseNodeModel::setInData(data, portIndex);

    // 你的额外逻辑，例如：
    // 更新通道数量
    updateChannelCount(data);

    // 如果需要重新刷新UI标题或端口标签，可在这里处理
    // ------------------------------------
    // ★★ 强制 QtNodes 重绘端口 UI ★★
    // 这里不是真的删除端口，只是触发 UI 重建
    // ------------------------------------
    emit portsAboutToBeDeleted(QtNodes::PortType::Out, 0, _desc.outputs.size() - 1);
    emit portsDeleted();
}
bool Split::portCaptionVisible(QtNodes::PortType type,
                               QtNodes::PortIndex index) const
{
    return true;   // 允许显示端口名称
}


