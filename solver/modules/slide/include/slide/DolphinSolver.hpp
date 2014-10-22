#ifndef SLIDE_DOLPHIN_SOLVER_HPP_
#define SLIDE_DOLPHIN_SOLVER_HPP_

#include "AnswerTreeBoard.hpp"
#include "Solver.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class DolphinSolver : public Solver
{
public:
	static bool verbose;

    int beamWidth = 200;

    using Solver::Solver;
    virtual ~DolphinSolver() override = default;

    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& board);

    template<int H, int W>
    AnswerTreeBoard<H, W> compute(const std::vector<AnswerTreeBoard<H, W>>& start, int y, int x, int h, int w);
};

} // end of namespace slide

#include "details/DolphinSolver.hpp"

#endif
