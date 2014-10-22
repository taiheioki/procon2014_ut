#include "StopWatch.hpp"

namespace util
{

StopWatch::hash_map StopWatch::orders({"", 0});
std::vector<std::tuple<StopWatch, int>> StopWatch::table;
std::stack<int> StopWatch::trace;

} // end of namespace util
