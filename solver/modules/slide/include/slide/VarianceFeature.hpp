#ifndef SLIDE_VARIANCE_FEATURE_HPP_
#define SLIDE_VARIANCE_FEATURE_HPP_

#include "PlayBoard.hpp"

namespace slide
{

template<int H, int W>
class VarianceFeature
{
private:
    int squaredSum;
    ushort sum_y;
    ushort sum_x;
    ushort cnt;

    void add(Point p)
    {
        squaredSum += p.y*p.y + p.x*p.x;
        sum_y += p.y;
        sum_x += p.x;
        ++cnt;
    }

    void remove(Point p)
    {
        squaredSum -= p.y*p.y + p.x*p.x;
        sum_y -= p.y;
        sum_x -= p.x;
        --cnt;
    }

public:
    VarianceFeature() = default;

    VarianceFeature(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        squaredSum = 0;
        sum_y = 0;
        sum_x = 0;
        cnt = 0;

        rep(i, board.height()) rep(j, board.width()){
            if(!board.isAligned(i, j) && board.selected != Point(i, j)){
                add(Point(i, j));
            }
        }
    }

    float operator()() const
    {
        return float(squaredSum) / cnt - float(int(sum_y)*sum_y + int(sum_x)*sum_x) / (cnt*cnt);
    }

    void move(const PlayBoardBase<H, W>& board, Direction dir)
    {
        const Point pre = board.selected - Point::delta(dir);
        const uchar id = board(pre);

        if(board.correctId(board.selected) != id){
            remove(board.selected);
        }
        if(board.correctId(pre) != id){
            add(pre);
        }
    }

    void select(const PlayBoardBase<H, W>& board, Point preSelect)
    {
        if(preSelect.y >= 0 && !board.isAligned(preSelect)){
            add(preSelect);
        }
        if(!board.isAligned(board.selected)){
            remove(board.selected);
        }
    }

    static float compute(const PlayBoardBase<H, W>& board)
    {
        return VarianceFeature(board)();
    }
};

} // end of namespace slide

#endif
