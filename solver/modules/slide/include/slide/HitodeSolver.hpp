#ifndef SLIDE_HITODE_SOLVER_HPP_
#define SLIDE_HITODE_SOLVER_HPP_

#include <vector>

#include "branch.hpp"
#include "HitodeBoard.hpp"
#include "Problem.hpp"
#include "Solver.hpp"

namespace slide
{

class HitodeSolver : public Solver
{
private:
    ull visitedNode;

    template<int H, int W>
    Answer bidirectionalAstar(const PlayBoard<H, W>& board);

    template<int H, int W>
    Board<H, W> inverseBoard(const Board<H, W>& board, std::vector<uchar>& table, std::vector<uchar>& invTable);

public:
    using Solver::Solver;
    virtual ~HitodeSolver() override = default;
    
    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& board);
};

} // end of namespace slide

#endif
