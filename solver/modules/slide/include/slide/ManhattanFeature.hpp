#ifndef SLIDE_MANHATTAN_FEATURE_HPP_
#define SLIDE_MANHATTAN_FEATURE_HPP_

#include "Point.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class ManhattanFeature
{
    /**************************************************************************
     * For non-static use
     *************************************************************************/

public:
    ushort manhattan;
    uchar selManhattan;

private:
    uchar selId;

public:
    ManhattanFeature() = default;

    template<int H, int W>
    explicit ManhattanFeature(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    template<int H, int W>
    void init(const PlayBoardBase<H, W>& board)
    {
        manhattan = compute(board);

        if(board.isSelected()){
            selId = board(board.selected);
            selManhattan = (board.selected - Point(selId)).l1norm();
        }
        else{
            selManhattan = 0;
        }
    }

    template<int H, int W>
    void move(const PlayBoardBase<H, W>& board, Direction dir)
    {
             if(dir == Direction::Up)    moveUp(board);
        else if(dir == Direction::Right) moveRight(board);
        else if(dir == Direction::Down)  moveDown(board);
        else                             moveLeft(board);
    }

    template<int H, int W>
    void moveUp(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Up);
        const Point dst1 = Point(selId);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        selManhattan += src1.y > dst1.y ? -1 : 1;
        manhattan    += src2.y < dst2.y ? -1 : 1;
    }

    template<int H, int W>
    void moveRight(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Right);
        const Point dst1 = Point(selId);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        selManhattan += src1.x < dst1.x ? -1 : 1;
        manhattan    += src2.x > dst2.x ? -1 : 1;
    }

    template<int H, int W>
    void moveDown(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Down);
        const Point dst1 = Point(selId);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        selManhattan += src1.y < dst1.y ? -1 : 1;
        manhattan    += src2.y > dst2.y ? -1 : 1;
    }

    template<int H, int W>
    void moveLeft(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Left);
        const Point dst1 = Point(selId);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        selManhattan += src1.x > dst1.x ? -1 : 1;
        manhattan    += src2.x < dst2.x ? -1 : 1;
    }

    template<int H, int W>
    void select(const PlayBoardBase<H, W>& board)
    {
        BOOST_ASSERT(board.isSelected());
        manhattan += selManhattan;
        selId = board(board.selected);
        selManhattan = (board.selected - Point(selId)).l1norm();
        manhattan -= selManhattan;
    }

    void unsetSelection()
    {
        manhattan += selManhattan;
        selManhattan = 0;
    }


    /**************************************************************************
     * For static use
     *************************************************************************/

    template<int H, int W>
    static int compute(const PlayBoardBase<H, W>& board)
    {
        int manhattan = 0;

        rep(i, board.height()) rep(j, board.width()){
            if(Point(i, j) != board.selected){
                manhattan += (Point(i, j) - Point(board(i, j))).l1norm();
            }
        }

        return manhattan;
    }

    template<int H, int W>
    static int delta(const Board<H, W>& board, Point src, Direction dir)
    {
             if(dir == Direction::Up)    return deltaUp(board, src);
        else if(dir == Direction::Right) return deltaRight(board, src);
        else if(dir == Direction::Down)  return deltaDown(board, src);
        else                             return deltaLeft(board, src);
    }

    template<int H, int W>
    static constexpr int deltaUp(const Board<H, W>& board, Point p)
    {
        return (p + Point::delta(Direction::Up)).y < Point(board(p)).y ? -1 : 1;
    }

    template<int H, int W>
    static constexpr int deltaRight(const Board<H, W>& board, Point p)
    {
        return (p + Point::delta(Direction::Right)).x > Point(board(p)).x ? -1 : 1;
    }

    template<int H, int W>
    static constexpr int deltaDown(const Board<H, W>& board, Point p)
    {
        return (p + Point::delta(Direction::Down)).y > Point(board(p)).y ? -1 : 1;
    }

    template<int H, int W>
    static constexpr int deltaLeft(const Board<H, W>& board, Point p)
    {
        return (p + Point::delta(Direction::Left)).x < Point(board(p)).x ? -1 : 1;
    }
};

} // end of namespace slide

#endif
