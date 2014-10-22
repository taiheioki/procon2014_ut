#ifndef UTIL_MATRIX_HPP
#define UTIL_MATRIX_HPP

#include <iostream>

namespace util{
namespace matrix{

// inverse matrix
// return false if determinant = 0
// currently support: 1x1, 2x2, 3x3, 4x4
template <int N>
bool inv_mat(const double /*in*/[N*N], double /*out*/[N*N]){
    std::cerr << "[inv_mat] " << N << "x" << N << " is not supported" << std::endl;
    return false;
}

template <>
bool inv_mat<1>(const double /*in*/[1], double /*out*/[1]);

template <>
bool inv_mat<2>(const double /*in*/[4], double /*out*/[4]);

template <>
bool inv_mat<3>(const double /*in*/[9], double /*out*/[9]);

template <>
bool inv_mat<4>(const double /*in*/[16], double /*out*/[16]);

bool inv_mat(const double [], double[], int);

void mult(const double A[], const double B[], double C[],
          const int N, const int M, const int L);

} // end of namespace matrix
} // end of namespace util

#endif // UTIL_MATRIX_HPP
