#ifndef SLIDE_BIT_BOARD_HPP_
#define SLIDE_BIT_BOARD_HPP_

#include <algorithm>
#include <iostream>
#include <type_traits>

#include <boost/assert.hpp>

#include "Board.hpp"
#include "Point.hpp"
#include "util/define.hpp"

namespace slide
{

template<int H, int W>
class FixedBitBoardVariables
{
protected:
    ushort data[H];

    FixedBitBoardVariables() = default;
    FixedBitBoardVariables(const FixedBitBoardVariables&) = default;

    FixedBitBoardVariables(int h, int w) {
        if(h != H || w != W){
            BOOST_ASSERT_MSG(false, "the wrong size!");
        }
    }

    constexpr int height() const {
        return H;
    }
    constexpr int width() const {
        return W;
    }
};

class FlexibleBitBoardVariables
{
protected:
    int h, w;
    ushort data[arraySize(Flexible)];

    FlexibleBitBoardVariables() = default;
    FlexibleBitBoardVariables(const FlexibleBitBoardVariables&) = default;
    constexpr FlexibleBitBoardVariables(int h, int w) : h(h), w(w), data() {}

    int& height() {
        return h;
    }
    int& width() {
        return w;
    }

public:
    constexpr int height() const {
        return h;
    }
    constexpr int width() const {
        return w;
    }
};

template<int H, int W>
using BitBoardVariables = typename std::conditional<H != Flexible, FixedBitBoardVariables<H, W>, FlexibleBitBoardVariables>::type;

template<int H_, int W_=H_>
class BitBoard : public BitBoardVariables<H_, W_>
{
private:
    using Base = BitBoardVariables<H_, W_>;
    using Base::data;

    /*
    (bitboard[y] << x) & 0x8000 == ture: fixed
    (bitboard[y] << x) & 0x8000 == false: not fixed
    */

    void fill(ushort value) {
        std::fill_n(data, arraySize(height()), value);
    }

public:
    static constexpr int H = H_;
    static constexpr int W = W_;
    static constexpr int arrayH = arraySize(H_);
    static constexpr int arrayW = arraySize(W_);

    using Base::height;
    using Base::width;

    /**************************************************************************
    * Constructors
    *************************************************************************/

    BitBoard()
    {
        static_assert((H != Flexible && W != Flexible) || (H == Flexible && W == Flexible),
            "Both of height() and W must be Flexible or must be non-Flexible simultaniously!");

        if(H != Flexible){
            reset();
        }
    }
    
    BitBoard(int h, int w) : Base(h, w) {
        reset();
    }

    void init(int h, int w)
    {
        if(h != H || w != W){
            BOOST_ASSERT_MSG(false, "the wrong size!");
        }
        reset();
    }

    /**************************************************************************
    * Accessors
    *************************************************************************/

    bool test(Point p) const
    {
        BOOST_ASSERT(p.isIn(height(), width()));
        return data[p.y] & (1 << p.x);
    }

    bool test(int y, int x) const
    {
        return test(Point(y, x));
    }

    bool testAllRow(int row) const
    {
        BOOST_ASSERT(0 <= row && row < height());
        return data[row] == (1<<width()) - 1;
    }

    bool testAllCol(int col) const
    {
        BOOST_ASSERT(0 <= col && col < width());
        rep(i, height()){
            if(!test(i, col)){
                return false;
            }
        }
        return true;
    }

    void reset()
    {
        fill(0);
    }

    void reset(Point p)
    {
        BOOST_ASSERT(p.isIn(height(), width()));
        data[p.y] &= ~(1 << p.x);
    }

    void reset(int y, int x)
    {
        return reset(y, x);
    }

    void set()
    {
        fill((1<<width()) - 1);
    }

    void set(Point p)
    {
        BOOST_ASSERT(p.isIn(height(), width()));
        data[p.y] |= 1 << p.x;
    }

    void set(int y, int x)
    {
        set(Point(y, x));
    }

    void set(Point p, bool flag)
    {
        BOOST_ASSERT(p.isIn(height(), width()));
        flag ? set(p) : reset(p);
    }

    void set(int y, int x, bool flag)
    {
        set(Point(y, x), flag);
    }

    // data[p.y] のbitを反転する
    void flip(Point p)
    {
        BOOST_ASSERT(p.isIn(height(), width()));
        data[p.y] ^= 1 << p.x;
    }

    void flip(int y, int x)
    {
        flip(Point(y, x));
    }

    int count() const
    {
        int cnt = 0;
        rep(i, height()) rep(j, width()){
            cnt += ((data[i] & (1<<j)) != 0);
        }
        return cnt;
    }

    int countZero() const
    {
        int cnt = 0;
        rep(i, height()) rep(j, width()){
            cnt += ((data[i] & (1<<j)) == 0);
        }
        return cnt;
    }

    int countRow(int row) const
    {
        BOOST_ASSERT(0 <= row && row < height());
        int cnt = 0;
        rep(i, width()){
            cnt += (data[row] & (1 << i)) != 0;
        }
        return cnt;
    }

    int countRow(int row, int x, int W) const
    {
        BOOST_ASSERT(0 <= row && row < height());
        int cnt = 0;
        reps(i, x, x + W){
            cnt += ((data[row] & (1 << i)) != 0 ) ;
        }
        return cnt;
    }

    int countRowZero(int row) const
    {
        BOOST_ASSERT(0 <= row && row < height());
        int cnt = 0;
        rep(i, width()){
            cnt += (data[row] & (1 << i)) == 0;
        }
        return cnt;
    }

    int countCol(int col) const
    {
        BOOST_ASSERT(0 <= col && col < width());
        int cnt = 0;
        rep(i, height()){
            cnt += (data[i] & (1 << col)) != 0;
        }
        return cnt;
    }

    int countCol(int col, int y, int H) const
    {
        BOOST_ASSERT(0 <= col && col < width());
        int cnt = 0;
        reps(i, y, y + H){
            cnt += (data[i] & (1 << col)) != 0;
        }
        return cnt;
    }

    int countColZero(int col) const
    {
        BOOST_ASSERT(0 <= col && col < width());
        int cnt = 0;
        rep(i, height()){
            cnt += (data[i] & (1 << col)) == 0;
        }
        return cnt;
    }

    void largetstRect(int& y, int& x, int& H, int& W) const
    {
        for(y=0;          y<height() && testAllRow(y); ++y);
        for(H=height()-1; H>=0       && testAllRow(H); --H);
        for(x=0;          x<width()  && testAllCol(x); ++x);
        for(W=width()-1;  W>=0       && testAllCol(W); --W);

        H -= y - 1;
        W -= x - 1;
    }

    /**************************************************************************
    * Operator
    *************************************************************************/

    bool operator()(Point p) const
    {
        return test(p);
    }

    bool operator()(int y, int x) const
    {
        return test(y, x);
    }

    friend std::ostream& operator<<(std::ostream& out, const BitBoard& board)
    {
        rep(i, board.height()){
            rep(j, board.width()){
                out << (board(i, j) ? '1' : '0');
            }
            out << '\n';
        }
        return out;
    }
};

template<>
inline void BitBoard<Flexible>::init(int h, int w)
{
    height() = h;
    width() = w;
    reset();
}

} // end of namespace slide

#endif
