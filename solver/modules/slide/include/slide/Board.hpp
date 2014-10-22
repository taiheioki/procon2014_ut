#ifndef SLIDE_BOARD_HPP_
#define SLIDE_BOARD_HPP_

#include <algorithm>
#include <iostream>
#include <type_traits>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include "Answer.hpp"
#include "Point.hpp"
#include "util/define.hpp"

namespace slide
{
constexpr int Flexible = -1;

inline constexpr int arraySize(int size) {
    return size == Flexible ? MAX_DIVISION_NUM : size;
}

template<int H, int W>
class FixedBoardBaseVariables
{
protected:
    uchar cell[H * W];

    FixedBoardBaseVariables() = default;
    FixedBoardBaseVariables(const FixedBoardBaseVariables&) = default;

#ifdef NDEBUG
    constexpr FixedBoardBaseVariables(int, int) : cell() {}
#else
    FixedBoardBaseVariables(int h, int w) {
        BOOST_ASSERT(h == H && w == W);
    }
#endif

    constexpr int height() const {
        return H;
    }
    constexpr int width() const {
        return W;
    }
};

class FlexibleBoardBaseVariables
{
protected:
    int h, w;
    uchar cell[arraySize(Flexible) * arraySize(Flexible)];

    FlexibleBoardBaseVariables() = default;
    FlexibleBoardBaseVariables(const FlexibleBoardBaseVariables&) = default;
    constexpr FlexibleBoardBaseVariables(int h, int w) : h(h), w(w), cell() {}

public:
    int& height() {
        return h;
    }
    int& width() {
        return w;
    }
    constexpr int height() const {
        return h;
    }
    constexpr int width() const {
        return w;
    }
};

template<int H, int W>
using BoardBaseVariables = typename std::conditional<H != Flexible, FixedBoardBaseVariables<H, W>, FlexibleBoardBaseVariables>::type;

template<int _H, int _W = _H>
class Board : public BoardBaseVariables<_H, _W>
{
private:
    using Base = BoardBaseVariables<_H, _W>;
    using Base::cell;

public:
    static constexpr int H = _H;
    static constexpr int W = _W;
    static constexpr int arrayH = arraySize(_H);
    static constexpr int arrayW = arraySize(_W);

    using Base::height;
    using Base::width;

    /**************************************************************************
     * Constructors
     *************************************************************************/

    constexpr Board() : Base() {
        static_assert((H != Flexible && W != Flexible) || (H == Flexible && W == Flexible),
            "Both of H and W must be Flexible or must be non-Flexible simultaniously!");
    }

    constexpr Board(int h, int w) : Base(h, w) {}

    Board(const Board<Flexible>& board);

    /**************************************************************************
     * Useful methods
     *************************************************************************/

    constexpr int area() const {
        return height() * width();
    }

    Point find(uchar index) const
    {
        const int pos = std::find(cell, cell + area(), index) - cell;
        BOOST_ASSERT_MSG(pos < area(), (boost::format("%0X is not found") % int(index)).str().c_str());
        return Point(uchar(pos), width());
    }

    constexpr uchar correctId(Point p) const {
        return p.toInt();
    }

    constexpr uchar correctId(int y, int x) const {
        return correctId(Point(y, x));
    }

    constexpr bool isAligned(Point p) const {
        return at(p) == correctId(p);
    }

    constexpr bool isAligned(int y, int x) const {
        return isAligned(Point(y, x));
    }

    bool isFinished() const {
        rep(i, height()) rep(j, width()){
            if(!isAligned(i, j)) return false;
        }
        return true;
    }

    constexpr bool isFlexible() const {
        return false;
    }

    /**************************************************************************
     * static functions
     *************************************************************************/

    static Board finishedState();
    static Board finishedState(int h, int w);
    static Board randomState();
    static Board randomState(int h, int w);


    /**************************************************************************
     * Accessors
     *************************************************************************/

    uchar& operator()(Point p) {
        return cell[p.toInt(width())];
    }

    uchar& operator()(int y, int x) {
        return operator()(Point(y, x));
    }

    uchar& at(Point p) {
        BOOST_ASSERT(p.isIn(height(), width()));
        return operator()(p);
    }

    uchar& at(int y, int x) {
        return at(Point(y, x));
    }

    constexpr uchar operator()(Point p) const {
        return cell[p.toInt(width())];
    }

    constexpr uchar operator()(int y, int x) const {
        return operator()(Point(y, x));
    }

    uchar at(Point p) const {
        BOOST_ASSERT(p.isIn(height(), width()));
        return operator()(p);
    }

    uchar at(int y, int x) const {
        return at(Point(y, x));
    }

    /**************************************************************************
     * I/O
     *************************************************************************/

    template<int H, int W>
    friend std::ostream& operator<<(std::ostream& out, const Board<_H, _W>& board);
};

} // end of namespace slide

#include "details/Board.hpp"

#endif
