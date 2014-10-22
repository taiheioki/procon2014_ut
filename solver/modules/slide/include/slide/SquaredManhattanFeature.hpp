#ifndef SLIDE_SQUARED_MANHATTAN_FEATURE_HPP_
#define SLIDE_SQUARED_MANHATTAN_FEATURE_HPP_

#include "Point.hpp"
#include "PlayBoard.hpp"
#include "util/sqrt_approx.hpp"

namespace slide
{

template<int H, int W>
class SquaredManhattanFeature
{
    /**************************************************************************
     * For non-static use
     *************************************************************************/

private:
    int manhattan;

public:
    SquaredManhattanFeature() = default;

    explicit SquaredManhattanFeature(const PlayBoardBase<H, W>& board)
    {
        init(board);
    }

    constexpr float operator()() const
    {
        return util::sqrt_approx(manhattan);
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

        manhattan += (src2.y < dst2.y ? -2 : 2) * (src1 - dst2).l1norm() - 1;
    }

    void moveRight(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Right);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += (src2.x > dst2.x ? -2 : 2) * (src1 - dst2).l1norm() - 1;
    }

    void moveDown(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Down);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += (src2.y > dst2.y ? -2 : 2) * (src1 - dst2).l1norm() - 1;
    }

    void moveLeft(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Left);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += (src2.x < dst2.x ? -2 : 2) * (src1 - dst2).l1norm() - 1;
    }

    void select(const PlayBoardBase<H, W>& board, Point preSelect)
    {
        BOOST_ASSERT(board.isSelected());
        if(preSelect.y >= 0){
            const int tmp = (preSelect - Point(board(preSelect))).l1norm();
            manhattan += tmp * tmp;
        }

        const int tmp = (board.selected - Point(board(board.selected))).l1norm();
        manhattan -= tmp * tmp;
    }

    /**************************************************************************
     * For static use
     *************************************************************************/

    static int compute(const PlayBoardBase<H, W>& board)
    {
        int manhattan = 0;

        rep(i, board.height()) rep(j, board.width()){
            if(Point(i, j) != board.selected){
                const int tmp = (Point(i, j) - Point(board(i, j))).l1norm();
                manhattan += tmp * tmp;
            }
        }

        return manhattan;
    }
};

template<int H, int W>
class RestrictedSquaredManhattanFeature
{
    /**************************************************************************
     * For non-static use
     *************************************************************************/

private:
    int manhattan;

    constexpr static int getCoefficient(Point p)
    {
        return p.isIn(y, x, h, w) ? 0 : 1;
    }

public:
    static int y, x, h, w;

    RestrictedSquaredManhattanFeature() = default;

    explicit RestrictedSquaredManhattanFeature(const PlayBoardBase<H, W>& board)
    {
        init(board);
    }

    constexpr float operator()() const
    {
        return util::sqrt_approx(manhattan);
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

        manhattan += getCoefficient(dst2) * ((src2.y < dst2.y ? -2 : 2) * (src1 - dst2).l1norm() - 1);
    }

    void moveRight(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Right);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += getCoefficient(dst2) * ((src2.x > dst2.x ? -2 : 2) * (src1 - dst2).l1norm() - 1);
    }

    void moveDown(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Down);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += getCoefficient(dst2) * ((src2.y > dst2.y ? -2 : 2) * (src1 - dst2).l1norm() - 1);
    }

    void moveLeft(const PlayBoardBase<H, W>& board)
    {
        // 選択中のセル
        const Point src1 = board.selected - Point::delta(Direction::Left);

        // 選択してないけど移動させられたセル
        const Point src2 = board.selected;
        const Point dst2 = Point(board(src1));

        manhattan += getCoefficient(dst2) * ((src2.x < dst2.x ? -2 : 2) * (src1 - dst2).l1norm() - 1);
    }

    void select(const PlayBoardBase<H, W>& board, Point preSelect)
    {
        BOOST_ASSERT(board.isSelected());
        if(preSelect.y >= 0){
            const Point dst(board(preSelect));
            const int tmp = (preSelect - Point(board(preSelect))).l1norm();
            manhattan += getCoefficient(dst) * tmp * tmp;
        }

        const Point dst(board(board.selected));
        const int tmp = (board.selected - dst).l1norm();
        manhattan -= getCoefficient(dst) * tmp * tmp;
    }

    /**************************************************************************
     * For static use
     *************************************************************************/

    static int compute(const PlayBoardBase<H, W>& board)
    {
        int manhattan = 0;

        rep(i, board.height()) rep(j, board.width()){
            if(Point(i, j) != board.selected){
                const Point dst(board(i, j));
                const int tmp = (Point(i, j) - dst).l1norm();
                manhattan += getCoefficient(dst) * tmp * tmp;
            }
        }

        return manhattan;
    }
};

template<int H, int W>
int RestrictedSquaredManhattanFeature<H, W>::y;

template<int H, int W>
int RestrictedSquaredManhattanFeature<H, W>::x;

template<int H, int W>
int RestrictedSquaredManhattanFeature<H, W>::h;

template<int H, int W>
int RestrictedSquaredManhattanFeature<H, W>::w;

} // end of namespace slide

#endif
