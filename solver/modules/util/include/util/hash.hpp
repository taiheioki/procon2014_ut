#ifndef UTIL_HASH_HPP_
#define UTIL_HASH_HPP_

#include <functional>
#include <string>
#include <utility>

namespace util
{

template<typename T>
struct hash;

template<>
struct hash<std::pair<std::string, int>>
{
    using result_type   = std::size_t;
    using argument_type = std::pair<std::string, int>;

    result_type operator()(const argument_type& arg) const
    {
        return std::hash<std::string>()(arg.first) ^ std::hash<int>()(arg.second);
    }
};

} // end of namespace util

#endif
