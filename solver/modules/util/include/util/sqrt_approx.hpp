#ifndef UTIL_SQRT_APPROX_HPP_
#define UTIL_SQRT_APPROX_HPP_

namespace util
{

inline float sqrt_approx(float z)
{
    union {
        float z;
        int val;
    } u;

    u.z = z;
    u.val -= 1 << 23;      // Subtract 2^m.
    u.val >>= 1;           // Divide by 2.
    u.val += 1 << 29;      // Add ((b + 1) / 2) * 2^m.

    return u.z;            // Interpret again as float
}

} // end of namespace util

#endif
