#include "branch.hpp"
#include "DolphinSolver.hpp"

namespace slide
{

bool DolphinSolver::verbose = false;

template<int H, int W>
void DolphinSolver::solve(const PlayBoard<H, W>& board)
{
    const std::vector<AnswerTreeBoard<H, W>> input = {AnswerTreeBoard<H, W>(AnswerTreeBoardBase<H, W>(board))};
    const AnswerTreeBoard<H, W> result = compute(input, board.height()/2-2, board.width()/2-2, 4, 4);
    Answer answer = result.buildAnswer();
    answer.optimize();
    onCreatedAnswer(answer);
}

template<>
void DolphinSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board)
{
    branch(make_solve_caller(*this), board);
}

void DolphinSolver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board));
}

double MeanCoefficient = 320;
double ManhattanCoefficient = 2.2;
double LinearCoefficient = 2.0;

} // end of namespace slide
