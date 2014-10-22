#ifndef SLIDE_DETAILS_BOARD_HPP_
#define SLIDE_DETAILS_BOARD_HPP_

#include <utility>
#include <boost/format.hpp>

#include "../Board.hpp"
#include "util/Random.hpp"

namespace slide
{

template<>
inline constexpr bool Board<Flexible>::isFlexible() const {
    return true;
}

template<int H, int W>
Board<H, W>::Board(const Board<Flexible>& board)
{
    BOOST_ASSERT(height() == board.height());
    BOOST_ASSERT(width() == board.width());

    rep(i, height()) rep(j, width()){
        at(i, j) = board(i, j);
    }
}

template<>
inline Board<Flexible>::Board(const Board<Flexible>& board) : FlexibleBoardBaseVariables(board) {
    *this = board;
}

template<int H, int W>
Board<H, W> Board<H, W>::finishedState()
{
    static_assert(H != Flexible && W != Flexible, "Both of H and W must not be Flexible");

    Board board;
    rep(i, H) rep(j, W){
        board(i, j) = board.correctId(i, j);
    }

    return board;
}

template<int H, int W>
Board<H, W> Board<H, W>::finishedState(int h, int w)
{
    BOOST_ASSERT((H == Flexible && W == Flexible) || (H == h && W == w));

    Board<H, W> board(h, w);
    rep(i, h) rep(j, w){
        board(i, j) = board.correctId(i, j);
    }

    return board;
}

template<int H, int W>
Board<H, W> Board<H, W>::randomState()
{
    static_assert(H != Flexible && W != Flexible, "Both of H and W must not be Flexible");

    Board board = finishedState();
    std::shuffle(board.cell, board.cell+board.area(), util::Random::getEngine());
    return board;
}

template<int H, int W>
Board<H, W> Board<H, W>::randomState(int h, int w)
{
    Board board = finishedState(h, w);
    std::shuffle(board.cell, board.cell+board.area(), util::Random::getEngine());
    return board;
}

template<int H, int W>
std::ostream& operator<<(std::ostream& out, const Board<H, W>& board)
{
    rep(i, board.height()){
        rep(j, board.width()){
            out << boost::format("|%02X") % int(board(i, j));
        }
        out << "|\n";
    }

    return out;
}

} // end of namespace Board

#endif
