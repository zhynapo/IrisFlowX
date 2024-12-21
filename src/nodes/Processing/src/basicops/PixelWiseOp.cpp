#include "PixelWiseOp.hpp"
#include "utils.hpp"

enum OPERATION
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_ABS,
    OP_MAX,
    OP_MIN,
    OP_AND,
    OP_OR ,
    OP_XOR,
    OP_CMP_EQ,
    OP_CMP_NE,
    OP_CMP_LT,
    OP_CMP_LE,
    OP_CMP_GT,
    OP_CMP_GE
};

const NodeDesc PixelWiseOp::desc =
{
    "PixelWiseOp",
    "PixelWiseOp",
    CAT_PROC_BASIC,

    {
        PortDesc::In("mat-A", NodeType::Mat),
        PortDesc::In("mat-B", NodeType::Mat)
    },

    // outputs
    {
        PortDesc::Out("mat", NodeType::Mat)
    },

    // parameters
    {
        ParamDesc::makeCombo("Op", "Operation", 
            QVector<QString>{ 
                QString("Add + "), 
                QString("Sub - "), 
                QString("Mul * "), 
                QString("Div / "), 
                QString("Abs "), 
                QString("Max"), 
                QString("Min"), 
                QString("And &"), 
                QString("Or  |"), 
                QString("Xor "), 
                QString("Equ"), 
                QString("Neq"),
                QString("Lt"),
                QString("Le"),
                QString("Gt"),
                QString("Ge")
            }, 
            QVector<int>{
                OP_ADD,
                OP_SUB,
                OP_MUL,
                OP_DIV,
                OP_ABS,
                OP_MAX,
                OP_MIN,
                OP_AND,
                OP_OR ,
                OP_XOR,
                OP_CMP_EQ,
                OP_CMP_NE,
                OP_CMP_LT,
                OP_CMP_LE,
                OP_CMP_GT,
                OP_CMP_GE
            },
            0)
    }
};

//REGISTER_NODE(PixelWiseOp)

PixelWiseOp::PixelWiseOp()
    : BaseNodeModel(desc)
{
}


void PixelWiseOp::process()
{
    auto mat_A = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    if (!mat_A)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat A = mat_A->mat();

    auto mat_B = std::dynamic_pointer_cast<MatNodeData>(_getInput(1));
    if (!mat_B)
    {
        setOutputData(0, nullptr);
        return;
    }

    cv::Mat B = mat_B->mat();

    // TODO: implement algorithm
    matchTypeSizeChannel(A, B);
    if (A.size() != B.size() || A.type() != B.type())
    {
        setOutputData(0, nullptr);
        return;
    }

    int op = parameterValue("Op").toInt();
    cv::Mat out;
    switch (op)
    {
    case OP_ADD:
        cv::add(A, B, out);
        break;
    case OP_SUB:
        cv::subtract(A, B, out);
        break;
    case OP_MUL:
        cv::multiply(A, B, out);
        break;
    case OP_DIV:
        cv::divide(A, B, out);
        break;
    case OP_ABS:
        cv::absdiff(A, B, out);
        break;
    case OP_MAX:
        cv::max(A, B, out);
        break;
    case OP_MIN:
        cv::min(A, B, out);
        break;
    case OP_AND:
        cv::bitwise_and(A, B, out);
        break;
    case OP_OR:
        cv::bitwise_or(A, B, out);
        break;
    case OP_XOR:
        cv::bitwise_xor(A, B, out);
        break;
    case OP_CMP_EQ:
        cv::compare(A, B, out, cv::CMP_EQ);
        break;
    case OP_CMP_NE:
        cv::compare(A, B, out, cv::CMP_NE);
        break;
    case OP_CMP_LT:
        cv::compare(A, B, out, cv::CMP_LT);
        break;
    case OP_CMP_LE:
        cv::compare(A, B, out, cv::CMP_LE);
        break;
    case OP_CMP_GT:
        cv::compare(A, B, out, cv::CMP_GT);
        break;
    case OP_CMP_GE:
        cv::compare(A, B, out, cv::CMP_GE);
        break;

    default:
        break;
    }

    setOutputData(0, std::make_shared<MatNodeData>(out));
}
