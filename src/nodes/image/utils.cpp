

#include "utils.hpp"

void matchTypeSizeChannel(cv::Mat& A, const cv::Mat& B)
{
    // 1. resize
    if (A.size() != B.size())
        cv::resize(A, A, B.size());

    // 2. channel convert
    if (A.channels() != B.channels())
    {
        if (A.channels() == 1 && B.channels() == 3)
            cv::cvtColor(A, A, cv::COLOR_GRAY2BGR);

        else if (A.channels() == 3 && B.channels() == 1)
            cv::cvtColor(A, A, cv::COLOR_BGR2GRAY);

        else if (A.channels() == 4 && B.channels() == 3)
            cv::cvtColor(A, A, cv::COLOR_BGRA2BGR);

        else if (A.channels() == 3 && B.channels() == 4)
            cv::cvtColor(A, A, cv::COLOR_BGR2BGRA);

        // 其他情况自行补
    }

    // 3. type convert
    if (A.type() != B.type())
        A.convertTo(A, B.type());
}