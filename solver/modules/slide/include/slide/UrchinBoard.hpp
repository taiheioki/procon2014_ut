#ifndef SLIDE_URCHIN_BOARD_HPP_
#define SLIDE_URCHIN_BOARD_HPP_

#include <algorithm>
#include <vector>

#include "AlignBoard.hpp"
#include "PlayBoard.hpp"
#include "util/define.hpp"

namespace slide
{

template<typename Base>
class UrchinBoardBase : public Base
{
public:
	// From Board
	using Base::H;
	using Base::W;
	using Base::arrayH;
	using Base::arrayW;
	using Base::height;
	using Base::width;
	using Base::at;
	using Base::isAligned;

	// From PlayBoard
	using Base::selected;

	// From FixBoard
	using Base::fixed;
	using Base::countAdjacentFixedCell;

private:
	uchar leftMost[arrayH];
	uchar rightMost[arrayH];

	// 候補の条件をみたすか試す
	bool fixedCheck(int py, int px, int y, int x, int sH, int sW) const;
	bool fixedCheck(int py, int px, int y, int x, int sH, int sW, int id) const;

	bool checkCorner(int py, int px, int y, int x, int sH, int sW) const;

public:
	UrchinBoardBase() = default;

	UrchinBoardBase(const PlayBoardBase<H, W>& board) {
		init(board);
	}

	void init(const PlayBoardBase<H, W>& board)
	{
		Base::init(board);
		std::fill_n(leftMost, arrayH, 0);
		std::fill_n(rightMost, arrayH, width()-1);
    }

    // 次に埋めることの出来る candidate を得る	
	void getCandidates(std::vector<Point>& cand) const;

	// cand を固定する
	void fixCandidate(Point cand);

	// cand の固定を解除する
	void unfixCandidate(Point cand);
};

template<typename Base>
using UrchinBoard = AlignBoard<UrchinBoardBase<Base>>;

} // end of namespace slide

#include "details/UrchinBoard.hpp"

#endif
