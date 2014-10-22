#ifndef SLIDE_LIZARD_SOLVER_HPP_
#define SLIDE_LIZARD_SOLVER_HPP_

#include "Solver.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class LizardSolver : public Solver
{
public:
    static bool verbose;

    using Solver::Solver;
    virtual ~LizardSolver() override = default;

    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& start);
};

} // end of namespace slide

#endif
