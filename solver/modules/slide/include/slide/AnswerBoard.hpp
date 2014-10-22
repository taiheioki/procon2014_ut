#ifndef SLIDE_ANSWER_BOARD_HPP_
#define SLIDE_ANSWER_BOARD_HPP_

#include "AnswerFeature.hpp"
#include "PlayBoard.hpp"

namespace slide
{

template<int H, int W = H>
class AnswerBoardBase : public PlayBoardBase<H, W>, public AnswerFeature
{
public:
    AnswerBoardBase() = default;
    
    explicit AnswerBoardBase(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        PlayBoardBase<H, W>::operator=(board);
        AnswerFeature::init();
    }

    void move(Direction dir)
    {
        PlayBoardBase<H, W>::move(dir);
        AnswerFeature::move(dir);
    }

    void select(Point newSelect)
    {
        PlayBoardBase<H, W>::select(newSelect);
        AnswerFeature::select(newSelect);
    }
};

template<int H, int W = H>
using AnswerBoard = PlayBoardUtility<AnswerBoardBase<H, W>>;

} // end of namespace slide

#endif
