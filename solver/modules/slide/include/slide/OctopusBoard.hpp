#ifndef SLIDE_OCTOPUS_BOARD_HPP_
#define SLIDE_OCTOPUS_BOARD_HPP_

#include <vector>
#include <algorithm>

#include "AlignBoard.hpp"
#include "util/color.hpp"
#include "util/define.hpp"

namespace slide
{

template<typename Base>
class OctopusBoardBase : public Base
{
private:
	// candidate かどうか確かめる
	bool isCandidate(int py, int px, int y, int x, int sH, int sW, int id) const;

	// 角の長さを計算する
	void lengthCorner(std::vector<int>& vertical, std::vector<int>& horizontal, int y, int x, int sH, int sW) const;

	bool checkCorner(int py, int px, int y, int x, int sH, int sW) const;

public:
	// From baord
	using Base::correctId;
	using Base::find;
	using Base::height;
	using Base::width;
	using Base::at;
	using Base::isAligned;

	// From PlayBoard
	using Base::selected;

	// From FixBoard
	using Base::countAdjacentFixedCell;
	using Base::fixed;

	using Base::Base;

	// 次に埋める事のできる candidates を得る
	void getCandidates(std::vector<Point>& cand) const;

	// cand を固定する
	void fixCandidate(Point cand) {
		fixed.set(cand);
	}

	// cand の固定を解除する
	void unfixCandidate(Point cand) {
		fixed.reset(cand);
	}
};

template<typename Base>
using OctopusBoard = AlignBoard<OctopusBoardBase<Base>>;

} // end of namespace slide

#include "details/OctopusBoard.hpp"

#endif
