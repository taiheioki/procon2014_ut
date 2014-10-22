#ifndef SLIDE_L2_SOLVER_HPP_
#define SLIDE_L2_SOLVER_HPP_

#include "Solver.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class L2Solver : public Solver
{
public:
    static bool verbose;

    using Solver::Solver;
    virtual ~L2Solver() override = default;
        
    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& start);
};

} // end of namespace slide

#endif
