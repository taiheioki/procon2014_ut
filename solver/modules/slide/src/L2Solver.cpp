#include "DolphinSolver.hpp"
#include "DragonSolver.hpp"
#include "L2Solver.hpp"
#include "branch.hpp"
#include "extract_board.hpp"
#include "KurageSolver.hpp"
#include "WhaleSolver.hpp"

#include "util/StopWatch.hpp"
#include "util/ThreadIndexManager.hpp"

#include <tbb/task_scheduler_init.h>

namespace slide
{

bool L2Solver::verbose = false;

template<int H, int W>
void L2Solver::solve(const PlayBoard<H, W>& board)
{
    Problem newProblem = problem;
    newProblem.selectionLimit = 2;
    DragonSolver solver(newProblem);
    DragonSolver::verbose = verbose;
    solver.numThreads = numThreads;
    solver.onCreatedAnswer = onCreatedAnswer;

    solver.solve(board);
}

void L2Solver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board));
}

} // end of namespace slide
