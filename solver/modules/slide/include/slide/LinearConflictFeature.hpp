#ifndef SLIDE_LINEAR_CONFLICT_FEATURE_HPP_
#define SLIDE_LINEAR_CONFLICT_FEATURE_HPP_

#include "PlayBoard.hpp"
#include "util/define.hpp"

namespace slide
{

template<int H, int W>
class LinearConflictFeature
{
    /**************************************************************************
     * For non-static use
     *************************************************************************/

private:
    int linearConflict;

public:
    LinearConflictFeature() = default;

    explicit LinearConflictFeature(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        linearConflict = compute(board);
    }

    int operator()() const
    {
        return linearConflict;
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
        const Point p = board.selected + Point::delta(Direction::Down);
        const Point dst(board(p));

        if(dst.y == board.selected.y){
            linearConflict -= computeHorizotally(board, board.selected, dst);
        }
        else if(dst.y == p.y){
            linearConflict += computeHorizotally(board, p, dst);
        }
    }

    void moveRight(const PlayBoardBase<H, W>& board)
    {
        const Point p = board.selected + Point::delta(Direction::Left);
        const Point dst(board(p));

        if(dst.x == board.selected.x){
            linearConflict -= computeVertically(board, board.selected, dst);
        }
        else if(dst.x == p.x){
            linearConflict += computeVertically(board, p, dst);
        }
    }

    void moveDown(const PlayBoardBase<H, W>& board)
    {
        const Point p = board.selected + Point::delta(Direction::Up);
        const Point dst(board(p));

        if(dst.y == board.selected.y){
            linearConflict -= computeHorizotally(board, board.selected, dst);
        }
        else if(dst.y == p.y){
            linearConflict += computeHorizotally(board, p, dst);
        }
    }
    
    void moveLeft(const PlayBoardBase<H, W>& board)
    {
        const Point p = board.selected + Point::delta(Direction::Right);
        const Point dst(board(p));

        if(dst.x == board.selected.x){
            linearConflict -= computeVertically(board, board.selected, dst);
        }
        else if(dst.x == p.x){
            linearConflict += computeVertically(board, p, dst);
        }
    }

    void select(const PlayBoardBase<H, W>& board, Point preSelect)
    {
        const Point preDst(board(preSelect));
        if(preDst.y == preSelect.y){
            linearConflict += computeHorizotally(board, preSelect, preDst);
        }
        if(preDst.x == preSelect.x){
            linearConflict += computeVertically(board, preSelect, preDst);
        }

        const Point newDst(board(board.selected));
        if(newDst.y == board.selected.y){
            linearConflict -= computeHorizotally(board, board.selected, newDst);
        }
        if(newDst.x == board.selected.x){
            linearConflict -= computeVertically(board, board.selected, newDst);
        }
    }

private:
    static int computeHorizotally(const Board<H, W>& board, Point p, Point dst_a)
    {
        int sum = 0;

        rep(i, p.x){
            const Point dst_b(board(p.y, i));
            sum += dst_b.y == p.y && dst_a.x < dst_b.x;
        }
        reps(i, p.x+1, board.width()){
            const Point dst_b(board(p.y, i));
            sum += dst_b.y == p.y && dst_a.x > dst_b.x;
        }

        return sum;
    }

    static int computeVertically(const Board<H, W>& board, Point p, Point dst_a)
    {
        int sum = 0;

        rep(i, p.y){
            const Point dst_b(board(i, p.x));
            sum += dst_b.x == p.x && dst_a.y < dst_b.y;
        }
        reps(i, p.y+1, board.height()){
            const Point dst_b(board(i, p.x));
            sum += dst_b.x == p.x && dst_a.y > dst_b.y;
        }

        return sum;
    }

    /**************************************************************************
     * For static use
     *************************************************************************/

public:
    static int compute(const PlayBoardBase<H, W>& board)
    {
        int sum = 0;

        // 横
        rep(i, board.height()) reps(j, 1, board.width()){
            if(board.selected == Point(i, j)){
                continue;
            }

            const Point dst_a(board(i, j));
            if(dst_a.y == i){
                rep(k, j){
                    const Point dst_b(board(i, k));
                    sum += dst_b.y == i && dst_a.x < dst_b.x && board.selected != Point(i, k);
                }
            }
        }

        // 縦
        rep(i, board.width()) reps(j, 1, board.height()){
            if(board.selected == Point(j, i)){
                continue;
            }

            const Point dst_a(board(j, i));
            if(dst_a.x == i){
                rep(k, j){
                    const Point dst_b(board(k, i));
                    sum += dst_b.x == i && dst_a.y < dst_b.y && board.selected != Point(k, i);
                }
            }
        }

        return sum;
    }
};

} // end of namespace slide

#endif
