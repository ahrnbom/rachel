#pragma once

#include <eigen3/Eigen/Geometry>
#include <string>

namespace rachel_utils {
template <typename T>
std::string format_matrix(const Eigen::MatrixBase<T>& m)
{
    static const Eigen::IOFormat f(4, 0, "  ", "\n", "[", "]", "\n[", "]");
    std::stringstream ss;
    ss << m.format(f);
    return ss.str();
}
}