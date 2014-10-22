#include <cmath>
#include <iostream>

#include "matrix.hpp"

namespace util{
namespace matrix{

constexpr const double EPS = 1e-9;

// 1x1
template <>
bool inv_mat<1>(const double in[1], double out[1])
{
    if(std::abs(in[0]) < EPS){
        return false;
    }

    out[0] = 1/in[0];

    return true;
}

// 2x2
// +--+--+
// +00+01+
// +--+--+
// +02+03+
// +--+--+
template <>
bool inv_mat<2>(const double in[4], double out[4])
{
    double det;

    det = in[0] * in[3] - (in[1] * in[2]);
    if(std::abs(det) < EPS){
        return false;
    }

    out[0] =  in[3];
    out[1] = -in[1];
    out[2] = -in[2];
    out[3] =  in[0];

    det = 1.0 / det;
    for(int i = 0; i < 4; i++){
        out[i] *= det;
    }

    return true;
}

// 3x3
// +--+--+--+
// +00+01+02+
// +--+--+--+
// +03+04+05+
// +--+--+--+
// +06+07+08+
// +--+--+--+
template <>
bool inv_mat<3>(const double in[9], double out[9])
{
    double a,b,c,d,e,f,g,h,i;
    double det;

    a = in[0]; b = in[1]; c = in[2];
    d = in[3]; e = in[4]; f = in[5];
    g = in[6]; h = in[7]; i = in[8];

    det = a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d;
    if(std::abs(det) < EPS){
        return false;
    }

    out[0] = (e*i-f*h);
    out[1] = -(b*i-c*h);
    out[2] = (b*f-c*e);

    out[3] = -(d*i-f*g);
    out[4] = (a*i-c*g);
    out[5] = -(a*f-c*d);

    out[6] = (d*h-e*g);
    out[7] = -(a*h-b*g);
    out[8] = (a*e-b*d);

    det = 1.0 / det;
    for(int i = 0; i < 9; i++){
        out[i] *= det;
    }

    return true;
}

// 4x4
// +--+--+--+--+
// +00+01+02+03+
// +--+--+--+--+
// +04+05+06+07+
// +--+--+--+--+
// +08+09+10+11+
// +--+--+--+--+
// +12+13+14+15+
// +--+--+--+--+
// from MESA implementation
template <>
bool inv_mat<4>(const double in[16], double out[16])
{
    double inv[16], det;
    int i;

    inv[0] = in[5]  * in[10] * in[15] -
             in[5]  * in[11] * in[14] -
             in[9]  * in[6]  * in[15] +
             in[9]  * in[7]  * in[14] +
             in[13] * in[6]  * in[11] -
             in[13] * in[7]  * in[10];

    inv[4] = -in[4]  * in[10] * in[15] +
              in[4]  * in[11] * in[14] +
              in[8]  * in[6]  * in[15] -
              in[8]  * in[7]  * in[14] -
              in[12] * in[6]  * in[11] +
              in[12] * in[7]  * in[10];

    inv[8] = in[4]  * in[9] * in[15] -
             in[4]  * in[11] * in[13] -
             in[8]  * in[5] * in[15] +
             in[8]  * in[7] * in[13] +
             in[12] * in[5] * in[11] -
             in[12] * in[7] * in[9];

    inv[12] = -in[4]  * in[9] * in[14] +
               in[4]  * in[10] * in[13] +
               in[8]  * in[5] * in[14] -
               in[8]  * in[6] * in[13] -
               in[12] * in[5] * in[10] +
               in[12] * in[6] * in[9];

    inv[1] = -in[1]  * in[10] * in[15] +
              in[1]  * in[11] * in[14] +
              in[9]  * in[2] * in[15] -
              in[9]  * in[3] * in[14] -
              in[13] * in[2] * in[11] +
              in[13] * in[3] * in[10];

    inv[5] = in[0]  * in[10] * in[15] -
             in[0]  * in[11] * in[14] -
             in[8]  * in[2] * in[15] +
             in[8]  * in[3] * in[14] +
             in[12] * in[2] * in[11] -
             in[12] * in[3] * in[10];

    inv[9] = -in[0]  * in[9] * in[15] +
              in[0]  * in[11] * in[13] +
              in[8]  * in[1] * in[15] -
              in[8]  * in[3] * in[13] -
              in[12] * in[1] * in[11] +
              in[12] * in[3] * in[9];

    inv[13] = in[0]  * in[9] * in[14] -
              in[0]  * in[10] * in[13] -
              in[8]  * in[1] * in[14] +
              in[8]  * in[2] * in[13] +
              in[12] * in[1] * in[10] -
              in[12] * in[2] * in[9];

    inv[2] = in[1]  * in[6] * in[15] -
             in[1]  * in[7] * in[14] -
             in[5]  * in[2] * in[15] +
             in[5]  * in[3] * in[14] +
             in[13] * in[2] * in[7] -
             in[13] * in[3] * in[6];

    inv[6] = -in[0]  * in[6] * in[15] +
              in[0]  * in[7] * in[14] +
              in[4]  * in[2] * in[15] -
              in[4]  * in[3] * in[14] -
              in[12] * in[2] * in[7] +
              in[12] * in[3] * in[6];

    inv[10] = in[0]  * in[5] * in[15] -
              in[0]  * in[7] * in[13] -
              in[4]  * in[1] * in[15] +
              in[4]  * in[3] * in[13] +
              in[12] * in[1] * in[7] -
              in[12] * in[3] * in[5];

    inv[14] = -in[0]  * in[5] * in[14] +
               in[0]  * in[6] * in[13] +
               in[4]  * in[1] * in[14] -
               in[4]  * in[2] * in[13] -
               in[12] * in[1] * in[6] +
               in[12] * in[2] * in[5];

    inv[3] = -in[1] * in[6] * in[11] +
              in[1] * in[7] * in[10] +
              in[5] * in[2] * in[11] -
              in[5] * in[3] * in[10] -
              in[9] * in[2] * in[7] +
              in[9] * in[3] * in[6];

    inv[7] = in[0] * in[6] * in[11] -
             in[0] * in[7] * in[10] -
             in[4] * in[2] * in[11] +
             in[4] * in[3] * in[10] +
             in[8] * in[2] * in[7] -
             in[8] * in[3] * in[6];

    inv[11] = -in[0] * in[5] * in[11] +
               in[0] * in[7] * in[9] +
               in[4] * in[1] * in[11] -
               in[4] * in[3] * in[9] -
               in[8] * in[1] * in[7] +
               in[8] * in[3] * in[5];

    inv[15] = in[0] * in[5] * in[10] -
              in[0] * in[6] * in[9] -
              in[4] * in[1] * in[10] +
              in[4] * in[2] * in[9] +
              in[8] * in[1] * in[6] -
              in[8] * in[2] * in[5];

    det = in[0] * inv[0] + in[1] * inv[4] + in[2] * inv[8] + in[3] * inv[12];

    if (std::abs(det) < EPS){
        return false;
    }

    det = 1.0 / det;

    for (i = 0; i < 16; i++){
        out[i] = inv[i] * det;
    }

    return true;
}

// this function does not any boundary check!
// A : NxM
// B : MxL
// C : NxL
void mult(const double A[], const double B[], double C[],
          const int N, const int M, const int L)
{
    for(int i = 0; i < N; i++){
        for(int j = 0; j < L; j++){
            C[i*N+j] = 0;
            for(int k = 0; k < M; k++){
                C[i*N+j] += A[i*N+k]*B[k*M+j];
            }
        }
    }
}

bool inv_mat(const double in[], double out[], const int n)
{
    switch(n){
        case 1:
            return inv_mat<1>(in,out);
        case 2:
            return inv_mat<2>(in,out);
        case 3:
            return inv_mat<3>(in,out);
        case 4:
            return inv_mat<4>(in,out);
        default:
            break;
    }
    std::cerr << "[inv_mat] " << n << "x" << n << " is not supported" << std::endl;
    return false;
}

} // end of namespace matrix
} // end of namespace util
