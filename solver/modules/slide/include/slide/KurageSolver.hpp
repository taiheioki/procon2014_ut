#ifndef SLIDE_KURAGE_SOLVER_HPP_
#define SLIDE_KURAGE_SOLVER_HPP_

#include <utility>
#include <vector>

#include "AnswerTreeBoard.hpp"
#include "branch.hpp"
#include "KurageBoard.hpp"
#include "Problem.hpp"
#include "Solver.hpp"

namespace slide
{

class KurageSolver : public Solver
{
private:
    ull visitedNode;

    void determineBeamWidth(
        double remainRatio,
        int h,
        int w,
        const std::vector<int>& reliability,
        std::vector<int>& width,
        int preMinLayer,
        int& minLayer,
        int& maxLayer
    );

    template<int H, int W>
    bool beamSearch(const PlayBoard<H, W>& start, int selectionLimit);

public:
    static bool verbose;
    int totalBeamWidth = 4000;
    bool retry = true;

    using Solver::Solver;
    virtual ~KurageSolver() override = default;
    
    void solve() override;

    template<int H, int W>
    void solve(const PlayBoard<H, W>& board, int selectionLimit);
};

} // end of namespace slide

#include "details/KurageSolver.hpp"

#endif
