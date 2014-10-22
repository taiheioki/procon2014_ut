#ifndef PARITY_FEATURE_HPP_
#define PARITY_FEATURE_HPP_

#include "PlayBoard.hpp"

namespace slide
{

template<int H, int W>
class ParityFeature
{
private:
    bool parity;
    bool selParity;

public:

    ParityFeature() = default;

    explicit ParityFeature(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        parity = false;

        rep(i, board.area()) rep(j, i) {
            const Point p1(i/board.width(), i%board.width());
            const Point p2(j/board.width(), j%board.width());
            parity ^= board(p1) < board(p2);
        }

        selParity = false;
        if(board.isSelected()){
            select(board);
        }
    }

    bool operator()() const {
        return parity;
    }

    void move()
    {
        selParity = !selParity;
    }

    void select(const PlayBoardBase<H, W>& board)
    {
        select((board.selected - Point(board(board.selected))).l1norm());
    }

    void select(int selManhattan)
    {
        parity ^= selParity;
        selParity = static_cast<bool>(selManhattan & 1);
        parity ^= selParity;
    }

    void unsetSelection()
    {
        select(0);
    }
};

} // end of namespace slide

#endif
