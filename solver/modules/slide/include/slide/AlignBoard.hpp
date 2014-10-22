#ifndef SLIDE_ALIGN_BOARD_HPP_
#define SLIDE_ALIGN_BOARD_HPP_

#include <iostream>
#include "Point.hpp"

namespace slide
{

template<typename Base>
class AlignBoard : public Base
{
private:
    bool alignLastOne(Point dst);

    // 溝にはまっているpieceを取り出す.
    bool extractSrcPiece(Point src, Point dst);

    // 適当に動ける方向に動く
    bool moveOneAnyDirection();

    // 隣接する2マスの位置を交換する
    bool swapAdjacentPiece(Point a, Point b);

public:
    using Base::Base;

    // from Board
    using Base::height;
    using Base::width;
    using Base::find;
    using Base::correctId;
    using Base::isAligned;
    using Base::at;

    // from PlayBoard
    using Base::selected;
    using Base::move;

    // from FixBoard
    using Base::fixed;
    using Base::moveToOptimally;
    using Base::trajectPiece;
    using Base::countAdjacentFixedCell;

    // from Octopus or Urchin
    using Base::fixCandidate;
    using Base::unfixCandidate;

    // dst を揃える
    bool align(Point dst);

    template<typename Base_>
    friend std::ostream& operator<<(std::ostream& out, const AlignBoard<Base_>& board);
};

} // end of namespace slide

#include "details/AlignBoard.hpp"

#endif
