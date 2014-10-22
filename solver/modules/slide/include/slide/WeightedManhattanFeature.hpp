#ifndef SLIDE_WEIGHTED_MANHATTAN_FEATURE_HPP_
#define SLIDE_WEIGHTED_MANHATTAN_FEATURE_HPP_

#include "Point.hpp"
#include "PlayBoard.hpp"
#include "util/abs.hpp"

namespace slide
{

template<int H, int W>
class WeightedManhattanFeature
{
    /**************************************************************************
     * For non-static use
     *************************************************************************/

private:
    float manhattan;

public:
    WeightedManhattanFeature() = default;

    explicit WeightedManhattanFeature(const PlayBoardBase<H, W>& board)
    {
        init(board);
    }

    constexpr float operator()() const
    {
        return manhattan;
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        manhattan = compute(board);
    }

    void move(const PlayBoardBase<H, W>& board, Direction dir)
    {
             if(dir == Direction::Up)    moveUp(board);
        else if(dir == Direction::Right) moveRight(board);
        else if(dir == Direction::Down)  moveDown(board);
        else                             moveLeft(board);
    }

    void moveUp(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Up);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += weight(board(src1)) * (src2.y < dst2.y ? -1 : 1);
    }

    void moveRight(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Right);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += weight(board(src1)) * (src2.x > dst2.x ? -2 : 2);
    }

    void moveDown(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Down);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += weight(board(src1)) * (src2.y > dst2.y ? -2 : 2);
    }

    void moveLeft(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Left);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += weight(board(src1)) * (src2.x < dst2.x ? -2 : 2);
    }

    void select(const PlayBoardBase<H, W>& board, Point preSelect)
    {
        BOOST_ASSERT(board.isSelected());
        if(preSelect.y >= 0){
            manhattan += weight(board(preSelect)) * (preSelect - Point(board(preSelect))).l1norm();
        }

        manhattan -= weight(board(board.selected)) * (board.selected - Point(board(board.selected))).l1norm();
    }

    /**************************************************************************
     * For static use
     *************************************************************************/

    static float weight(uchar id)
    {
        const Point dst(id);
        return (util::abs(dst.y - (H-1)*0.5f) + util::abs(dst.x - (W-1)*0.5f)) / (H+W-2) + 1; // ∈ [1, 2]
    }

    static float compute(const PlayBoardBase<H, W>& board)
    {
        float manhattan = 0;

        rep(i, board.height()) rep(j, board.width()){
            if(Point(i, j) != board.selected){
                manhattan += weight(board(i, j)) * (Point(i, j) - Point(board(i, j))).l1norm();
            }
        }

        return manhattan;
    }
};

} // end of namespace slide

#endif
