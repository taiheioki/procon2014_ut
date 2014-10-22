#ifndef SLIDE_DETAILS_PLAYBOARD_HPP_
#define SLIDE_DETAILS_PLAYBOARD_HPP_

#include "../PlayBoard.hpp"
#include "util/color.hpp"

namespace slide
{

template<int H, int W>
std::ostream& operator<<(std::ostream& out, const PlayBoardBase<H, W>& board)
{
    rep(i, board.height()){
        rep(j, board.width()){
            out << "|";

            bool changed;
            if(board.selected == Point(i, j)){
                out << util::ForeRed;
                changed = true;
            }
            else{
                changed = false;
            }

            out << boost::format("%02X") % int(board(i, j));

            if(changed){
                out << util::ForeDefault;
            }
        }
        out << "|\n";
    }

    return out;
}

} // end of namespace slide

#endif
