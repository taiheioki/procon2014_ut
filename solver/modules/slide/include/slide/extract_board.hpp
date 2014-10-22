#ifndef SLIDE_EXTRACT_BOARD_HPP_
#define SLIDE_EXTRACT_BOARD_HPP_

#include <boost/assert.hpp>

#include "Answer.hpp"
#include "PlayBoard.hpp"

namespace slide
{

template<int H, int W, int h, int w>
inline void extractBoard(const PlayBoardBase<H, W>& src, PlayBoardBase<h, w>& dst, int y, int x)
{
    BOOST_ASSERT(y+h <= H && x+w <= W);

    rep(i, h) rep(j, w){
        dst(i, j) = dst.correctId(Point(src(i+y, j+x)) - Point(y, x));
    }

    if(src.isSelected()){
        dst.selected = src.selected - Point(y, x);
        BOOST_ASSERT(dst.selected.isIn(h, w));
    }
    else{
        dst.selected = src.selected;
    }
}

inline void appendAnswer(Answer& answer, const Answer& append, int y, int x)
{
    for(const Move move : append){
        if(move.isSelection){
            answer.emplace_back(move.getSelected() + Point(y, x));
        }
        else{
            answer.push_back(move);
        }
    }
}

} // end of namespace slide

#endif
