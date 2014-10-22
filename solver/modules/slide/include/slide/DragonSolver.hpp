#ifndef SLIDE_DRAGON_SOLVER_HPP_
#define SLIDE_DRAGON_SOLVER_HPP_

#include "Solver.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class DragonSolver : public Solver
{
public:
    static bool verbose;

    using Solver::Solver;
    virtual ~DragonSolver() override = default;
        
    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& start);
};

} // end of namespace slide

#endif
