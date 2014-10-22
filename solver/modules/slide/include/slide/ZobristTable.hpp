#ifndef ZOBRIST_HASH_HPP_
#define ZOBRIST_HASH_HPP_

#include <type_traits>
#include <boost/assert.hpp>

#include "Board.hpp"
#include "Point.hpp"
#include "util/define.hpp"
#include "util/Random.hpp"

namespace slide
{

template<int H, int W>
class FixedZobristTableVariables
{
protected:
    ull table[MAX_DIVISION_NUM * MAX_DIVISION_NUM][H][W];    // [id][pos.y][pos.x]
    ull selectedTable[MAX_DIVISION_NUM * MAX_DIVISION_NUM];  // [id]

    FixedZobristTableVariables() = default;
    FixedZobristTableVariables(const FixedZobristTableVariables&) = default;

    constexpr int height() const {
        return H;
    }
    constexpr int width() const {
        return W;
    }

#ifdef NDEBUG
    void resize(int, int) {}
#else
    void resize(int h, int w) {
    	BOOST_ASSERT(h == H && w == W);
    }
#endif
};

class FlexibleZobristTableVariables
{
protected:
    int h, w;
    ull table[MAX_DIVISION_NUM * MAX_DIVISION_NUM][MAX_DIVISION_NUM][MAX_DIVISION_NUM];
    ull selectedTable[MAX_DIVISION_NUM * MAX_DIVISION_NUM];

    FlexibleZobristTableVariables() = default;
    FlexibleZobristTableVariables(const FlexibleZobristTableVariables&) = default;
    constexpr FlexibleZobristTableVariables(int h, int w) : h(h), w(w), table(), selectedTable() {}

public:
    constexpr int height() const {
        return h;
    }
    constexpr int width() const {
        return w;
    }
    void resize(int h, int w) {
        this->h = h;
        this->w = w;
    }
};

template<int H, int W>
using ZobristTableVariables = typename std::conditional<H != Flexible, FixedZobristTableVariables<H, W>, FlexibleZobristTableVariables>::type;

template<int _H, int _W = _H>
class ZobristTable : ZobristTableVariables<_H, _W>
{
private:
    using Base = ZobristTableVariables<_H, _W>;
    using Base::table;
    using Base::selectedTable;

public:
    static constexpr int H = _H;
    static constexpr int W = _W;

    using Base::height;
    using Base::width;

    ZobristTable(){
        static_assert((H != Flexible && W != Flexible) || (H == Flexible && W == Flexible),
            "Both of H and W must be Flexible or must be non-Flexible simultaniously!");
        init();
    };

    ZobristTable(int h, int w) {
        init(h, w);
    }

    void init()
    {
        rep(i, MAX_DIVISION_NUM * MAX_DIVISION_NUM) rep(y, height()) rep(x, width()){
            table[i][y][x] = util::Random::nextULL();
        }
        rep(i, MAX_DIVISION_NUM * MAX_DIVISION_NUM){
        	selectedTable[i] = util::Random::nextULL();
        }
    }

    void init(int h, int w)
    {
        Base::resize(h, w);
        init();
    }

    ull look(uchar id, Point pos) const
    {
        BOOST_ASSERT(Point(id).isIn(height(), width()));
        BOOST_ASSERT(pos.isIn(height(), width()));

        return table[id][pos.y][pos.x];
    }

    ull lookSelected(uchar id) const
    {
        BOOST_ASSERT(Point(id).isIn(H, W));
        return selectedTable[id];
    }

    ull& look(uchar id, Point pos)
    {
        BOOST_ASSERT(Point(id).isIn(height(), width()));
        BOOST_ASSERT(pos.isIn(height(), width()));

        return table[id][pos.y][pos.x];
    }

    ull& lookSelected(uchar id)
    {
        BOOST_ASSERT(Point(id).isIn(H, W));
        return selectedTable[id];
    }
};

} // end of namespace slide

#endif
