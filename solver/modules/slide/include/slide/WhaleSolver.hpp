#ifndef SLIDE_WHALE_SOLVER_HPP_
#define SLIDE_WHALE_SOLVER_HPP_

#include <utility>
#include <vector>

#include "AnswerTreeBoard.hpp"
#include "branch.hpp"
#include "WhaleBoard.hpp"
#include "Problem.hpp"
#include "Solver.hpp"

namespace slide
{

class WhaleSolver : public Solver
{
public:
    static bool verbose;
    int totalBeamWidth = 4000;
    int offset_x;
    int offset_y;

    using Solver::Solver;
    virtual ~WhaleSolver() override = default;
    void solve() override {
        BOOST_ASSERT(false);
    }

    template<int H, int W>
    void solve(const PlayBoard<H, W>&, int){
        BOOST_ASSERT(false);
    }

    template<int H, int W>
    std::vector<AnswerTreeBoard<H, W>> decreaseManhattan(const PlayBoard<H, W>& start, int selectionLimit, int y, int x, int h, int w);

};

} // end of namespace slide

#include "details/WhaleSolver.hpp"

#endif
