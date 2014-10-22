#ifndef SLIDE_STRAIGHT_SOLVER_HPP_
#define SLIDE_STRAIGHT_SOLVER_HPP_

#include "Solver.hpp"
#include "AnswerBoard.hpp"

namespace slide
{

class StraightSolver : public Solver
{
private:
	void alignRow(AnswerBoard<Flexible>& board);

	// [y, y+height) × [x, x+width) の中を動く
	void moveTargetPiece(AnswerBoard<Flexible>& board, Point src, Point dst, int y, int x, int height, int width);

	void align2x2(AnswerBoard<Flexible>& board);

	AnswerBoard<Flexible> cropTo(
		const AnswerBoard<Flexible>& src, int y, int x, int height, int width, Direction topSide, bool yflip, bool xflip);

	void restore(const AnswerBoard<Flexible>& src, AnswerBoard<Flexible>& dst, int y, int x, Direction topSide, bool yflip, bool xflip);

public:
	using Solver::Solver;
	void solve() override;
	inline virtual ~StraightSolver() override = default;
};

} // end of namespace slide

#endif
