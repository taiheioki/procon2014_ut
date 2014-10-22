#ifndef SLIDE_COUNT_BOARD_HPP_
#define SLIDE_COUNT_BOARD_HPP_

#include "AnswerTreeFeature.hpp"
#include "PlayBoard.hpp"

namespace slide
{

template<int H, int W>
class AnswerTreeBoardBase : public PlayBoardBase<H, W>, public AnswerTreeFeature
{
public:
    int swappingCount;

	AnswerTreeBoardBase() = default;

	explicit AnswerTreeBoardBase(const PlayBoardBase<H, W>& board) {
		init(board);
	}

	void init(const PlayBoardBase<H, W>& board)
	{
		PlayBoardBase<H, W>::operator=(board);
		AnswerTreeFeature::init();
		swappingCount = 0;
	}

	void move(Direction dir)
	{
		PlayBoardBase<H, W>::move(dir);
		AnswerTreeFeature::move(dir);
		swappingCount++;
	}

	void select(Point newSelect)
	{
		PlayBoardBase<H, W>::select(newSelect);
		AnswerTreeFeature::select(newSelect);
	}
};

template<int H, int W>
using AnswerTreeBoard = PlayBoardUtility<AnswerTreeBoardBase<H, W>>;
 
} // end of namespace

#endif
