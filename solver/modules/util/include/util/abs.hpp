#ifndef UTIL_ABS_HPP_
#define UTIL_ABS_HPP_

namespace util
{

template<typename T>
inline constexpr T abs(const T& x) {
    return x<0 ? -x : x;
}

} // end of namespace util

#endif
