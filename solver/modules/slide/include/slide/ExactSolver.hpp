#ifndef SLIDE_EXACT_SOLVER_HPP_
#define SLIDE_EXACT_SOLVER_HPP_

#include "Solver.hpp"
#include "AdmissibleBoard.hpp"
#include "Problem.hpp"

namespace slide
{

class ExactSolver : public Solver
{
private:
    Answer answer;
    ull visitedNode;

    template<int H, int W>
    void IDAstar(const PlayBoard<H, W>& board);

    template<int H, int W>
    int startDFS(const AdmissibleBoard<H, W>& board, int lb);

    template<int H, int W>
    int DFS(AdmissibleBoard<H, W>& board, int lb, Move preMove);

public:
    using Solver::Solver;
    virtual ~ExactSolver() override = default;

    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& board);
};

} // end of namespace slide

#endif
