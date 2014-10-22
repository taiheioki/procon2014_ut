#ifndef SLIDE_SHARK_SOLVER_HPP_
#define SLIDE_SHARK_SOLVER_HPP_

#include <vector>
#include <iostream>

#include "Solver.hpp"
#include "SharkBoard.hpp"

namespace slide
{

class SharkSolver : public Solver
{
private:
	std::vector<slide::Point> cand;

	template<int H, int W>
	void greedy(const PlayBoard<H, W>& board);

public:
	static bool verbose;

	using Solver::Solver;
    virtual ~SharkSolver() override = default;

    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& board);
    
};

} // end of namespace slide

#endif
