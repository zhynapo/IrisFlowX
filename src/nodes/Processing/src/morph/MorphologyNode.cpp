#include "MorphologyNode.hpp"

const NodeDesc MorphologyNode::desc =
{
    "Morphology",
    "Morphology",
    CAT_PROC_MORPH,

    // inputs
    {
        PortDesc::In("mat", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeCombo("op", "Operation",
            QVector<QString>{
                QString("Erode"),
                QString("Dilate"),
                QString("Open"),
                QString("Close"),
                QString("Gradient"),
                QString("Tophat"),
                QString("Blackhat"),
                QString("HITMISS ")
            },
            QVector<int>{
                0, 1, 2, 3, 4, 5, 6, 7
            },
            0),
        ParamDesc::makeCombo("shape", "Kernel Shape",
            QVector<QString>{
                QString("Rect"),
                QString("Ellipse"),
                QString("Cross")
            },
            QVector<int>{
                cv::MORPH_RECT,
                cv::MORPH_ELLIPSE,
                cv::MORPH_CROSS,
            },
            cv::MORPH_RECT),

        ParamDesc::makeInt("ksize", "Kernel Size", 3)
            .setRange(1, 81)
            .setStep(2)      // 形态学核建议奇数
    }
};


MorphologyNode::MorphologyNode()
    : BaseNodeModel(desc)
{
}

cv::Mat MorphologyNode::applyMorphPerChannel(const cv::Mat& src, int op, int shape, int ksize)
{
    std::vector<cv::Mat> chs;
    cv::split(src, chs);

    cv::Mat kernel = cv::getStructuringElement(shape, cv::Size(ksize, ksize));

    for (auto& c : chs)
    {
        if (op == 0) cv::erode(c, c, kernel);
        else if (op == 1) cv::dilate(c, c, kernel);
        else cv::morphologyEx(c, c, op , kernel);
        // op+2: (0,1 的情况已处理，2->open=2, etc)
    }

    cv::Mat out;
    cv::merge(chs, out);
    return out;
}

void MorphologyNode::process()
{
    auto matData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!matData)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat input = matData->mat();
    if (input.empty())
    {
        setOutputData(0, nullptr);
        return;
    }

    int op = parameterValue("op").toInt();
    int shape = parameterValue("shape").toInt();
    int ksize = parameterValue("ksize").toInt();

    if (ksize < 1) ksize = 1;
    if (ksize % 2 == 0) ksize++;

    cv::Mat output;

    // 多通道独立处理版本（推荐）
    if (input.channels() == 3)
    {
        output = applyMorphPerChannel(input, op, shape, ksize);
    }
    else
    {
        cv::Mat kernel = cv::getStructuringElement(shape, cv::Size(ksize, ksize));

        switch (op)
        {
        case 0: cv::erode(input, output, kernel); break;
        case 1: cv::dilate(input, output, kernel); break;
        case 2: cv::morphologyEx(input, output, cv::MORPH_OPEN, kernel); break;
        case 3: cv::morphologyEx(input, output, cv::MORPH_CLOSE, kernel); break;
        case 4: cv::morphologyEx(input, output, cv::MORPH_GRADIENT, kernel); break;
        case 5: cv::morphologyEx(input, output, cv::MORPH_TOPHAT, kernel); break;
        case 6: cv::morphologyEx(input, output, cv::MORPH_BLACKHAT, kernel); break;
        default:
            output = input.clone();
        }
    }

    setOutputData(0, std::make_shared<MatNodeData>(output));
}