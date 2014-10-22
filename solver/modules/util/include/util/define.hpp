#ifndef UTIL_DEFINE_HPP_
#define UTIL_DEFINE_HPP_

/*****************************************************************************
 * Macros for compiler detection
 *****************************************************************************/

// Clang/LLVM: __clang__
// GNU G++:    __gcc__
// Intel ICPC: __icc__

#ifndef __clang__
#ifdef __ICC
#define __icc__
#elif defined(__GNUC__)
#define __gcc__
#endif
#endif


/*****************************************************************************
 * Macro
 *****************************************************************************/

#define reps(i, f, n) for(int i=int(f); i<int(n); ++i)
#define rep(i, n) reps(i, 0, n)


/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

using uchar  = unsigned char;
using ushort = unsigned short;
using uint   = unsigned int;
using ll     = long long;
using ull    = unsigned long long ;


/*****************************************************************************
 * Constats
 *****************************************************************************/

// General
constexpr int MAX_SUBMITTION_TIME     = 10;

// Jigsaw Puzzle
constexpr int MIN_IMAGE_SIZE          = 32;
constexpr int MAX_IMAGE_SIZE          = 1024;

constexpr int MIN_DIVISION_NUM        = 2;
constexpr int MAX_DIVISION_NUM        = 16;

constexpr int MIN_FRAGMENT_IMAGE_SIZE = 16;
constexpr int MAX_FRAGMENT_IMAGE_SIZE = 128;

// Slide Puzzle
constexpr int MIN_SELECTION_LIMIT     = 2;
constexpr int MAX_SELECTION_LIMIT     = 16;

constexpr int MIN_SELECTION_COST      = 1;
constexpr int MAX_SELECTION_COST      = 300;

constexpr int MIN_SWAPPING_COST       = 1;
constexpr int MAX_SWAPPING_COST       = 100;

constexpr int TIME_RATE               = 100;

#endif
