#include "utils.hpp"

// Additional helper implementations can be added here.

namespace nodes {

// Example non-inline implementation (kept simple)
cv::Mat toGray_impl(const cv::Mat &src)
{
    return toGray(src);
}

} // namespace nodes
