#ifndef UTIL_COLOR_HPP_
#define UTIL_COLOR_HPP_

namespace util
{

constexpr const char* ForeBlack   = "\x1b[30m";
constexpr const char* ForeRed     = "\x1b[31m";
constexpr const char* ForeGreen   = "\x1b[32m";
constexpr const char* ForeYellow  = "\x1b[33m";
constexpr const char* ForeBlue    = "\x1b[34m";
constexpr const char* ForeMagenta = "\x1b[35m";
constexpr const char* ForeCyan    = "\x1b[36m";
constexpr const char* ForeGray    = "\x1b[37m";
constexpr const char* ForeDefault = "\x1b[39m";

constexpr const char* BackBlack   = "\x1b[40m";
constexpr const char* BackRed     = "\x1b[41m";
constexpr const char* BackGreen   = "\x1b[42m";
constexpr const char* BackYellow  = "\x1b[43m";
constexpr const char* BackBlue    = "\x1b[44m";
constexpr const char* BackMagenta = "\x1b[45m";
constexpr const char* BackCyan    = "\x1b[46m";
constexpr const char* BackGray    = "\x1b[47m";
constexpr const char* BackDefault = "\x1b[49m";

constexpr const char* UnderBar    = "\x1b[4m";
constexpr const char* Highlight   = "\x1b[1m";
constexpr const char* FlipColor   = "\x1b[7m";
constexpr const char* Default     = "\x1b[0m";

} // end of namespace util

#endif
