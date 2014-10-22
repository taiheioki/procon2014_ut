#ifndef SLIDE_PLAY_BOARD_HPP_
#define SLIDE_PLAY_BOARD_HPP_

#include <utility>

#include <boost/assert.hpp>

#include "Board.hpp"
#include "PlayBoardUtility.hpp"

namespace slide
{

template<int H, int W = H>
class PlayBoardBase : public Board<H, W>
{
public:
    using Base = Board<H, W>;

	using Base::at;
	using Base::height;
	using Base::width;

    Point selected = Point(-1, -1);

    /**************************************************************************
     * Constructors
     *************************************************************************/

	using Board<H, W>::Board;
	
    PlayBoardBase() = default;

	PlayBoardBase(const Base& board) {
		init(board);
	}
	
	void init(const Base& board)
    {
		Base::operator=(board);
		selected.y = -1;
	}

    /**************************************************************************
     * The move operation
     *************************************************************************/

	void move(Direction dir)
    {
		BOOST_ASSERT(isValidMove(dir));
		const Point next = selected + Point::delta(dir);
		std::swap(at(selected), at(next));
		selected = next;
	}

	constexpr bool isValidMove(Direction dir) const {
		return isSelected() && (selected + Point::delta(dir)).isIn(height(), width());
	}

    /**************************************************************************
     * The select operation
     *************************************************************************/

	void select(Point newSelect)
    {
		BOOST_ASSERT(newSelect.isIn(height(), width()));
		selected = newSelect;
	}

	constexpr bool isSelected() const {
		return selected.y >= 0;
	}

	template<int _H, int _W>
	friend std::ostream& operator<<(std::ostream& out, const PlayBoardBase<_H, _W>& board);
};

template<int H, int W = H>
using PlayBoard = PlayBoardUtility<PlayBoardBase<H, W>>;

} // end of namespace slide

#include "details/PlayBoard.hpp"

#endif
