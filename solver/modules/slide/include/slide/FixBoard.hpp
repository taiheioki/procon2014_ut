#ifndef SLIDE_FIX_BOARD_HPP_
#define SLIDE_FIX_BOARD_HPP_

#include <iostream>

#include "AnswerBoard.hpp"
#include "AnswerTreeBoard.hpp"
#include "BitBoard.hpp"

namespace slide
{

template<typename Base>
class FixBoard : public Base
{
public:
    using Base::height;
    using Base::width;
    using Base::selected;
    using Base::isSelected;
    using Base::move;
    using Base::moveUp;
    using Base::moveRight;
    using Base::moveDown;
    using Base::moveLeft;
    using Base::moveHorizontally;
    using Base::moveVertically;

    using Base::H;
    using Base::W;
    using Base::arrayH;
    using Base::arrayW;

    BitBoard<H, W> fixed;

    /**************************************************************************
     * Constructors
     *************************************************************************/

    FixBoard() = default;

    explicit FixBoard(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        Base::init(board);
        fixed.init(board.height(), board.width());
    }

    /**************************************************************************
     * The high-tech move operation
     *************************************************************************/

    // dst まで選択セルを移動する。
    // 戻り値は成功したか失敗したかを表す。失敗した場合、盤面を操作しない。
    bool moveTo(Point dst);
    bool moveToOptimally(Point dst);

    // dst まで選択セルを移動するのにかかる最小の移動回数を計算する。-1の場合、移動不可能であることを表す。
    // この関数は盤面を操作しない。
    int countMoveTo(Point dst) const;
    int countMoveTo(Point src, Point dst) const;

    // src を dst まで最短経路で運ぶ。
    // 戻り値は成功したか失敗したかを表す。失敗した場合、盤面を操作しない。   
    bool trajectPiece(Point src, Point dst);
    
    // 一隣接の fixed の数を数える
    uchar countAdjacentFixedCell(Point p) const;

    template<typename Base_>
    friend std::ostream& operator<<(std::ostream& out, const FixBoard<Base_>& board);

private:
    int countMoveToBySearch(Point src, Point dst) const;
    int countMoveToTwoNeighbor(Point src, Point dst) const;
    // void countMoveTo(const std::vector<Point>& dst, std::vector<int>& ret) const;
    // void countMoveTo(Point src, const std::vector<Point>& dst, std::vector<int>& ret) const;

    bool searchMove(Point src, Point dst, char pre[arrayH][arrayW]) const;
    bool searchMoveOptimally(Point src, Point dst, char pre[arrayH][arrayW]) const;

    bool moveToOptimallyBySearch(Point dst);
    bool moveToTwoNeighborOptimally(Point dst);

    template<uchar Rot_>
    bool moveToLinearTwoNeighborMoreOptimally();

    bool moveToTwoNeighborMoreOptimally(Point dst);
}; 

template<int H, int W>
using FixAnswerBoard = FixBoard<AnswerBoard<H, W>>;

template<int H, int W>
using FixAnswerTreeBoard  = FixBoard<AnswerTreeBoard<H, W>>;

} // end of namespace slide

#include "details/FixBoard.hpp"

#endif
