#include "branch.hpp"
#include "extract_board.hpp"
#include "DolphinSolver.hpp"
#include "KurageSolver.hpp"
#include "LizardSolver.hpp"

#include "util/StopWatch.hpp"
#include "util/ThreadIndexManager.hpp"

#include <tbb/task_scheduler_init.h>

namespace slide
{

bool LizardSolver::verbose = false;

template<int H, int W>
void LizardSolver::solve(const PlayBoard<H, W>& start)
{
    util::StopWatch::start("total");

    // 前準備
    if(numThreads == -1){
        numThreads = tbb::task_scheduler_init::default_num_threads();
    }
    tbb::task_scheduler_init init(numThreads);
    util::ThreadIndexManager::clear();

    AnswerLinearTree::nodes.clear();
    AnswerLinearTree::nodes.resize(numThreads);
    rep(i, numThreads){
        AnswerLinearTree::nodes[i].reserve(400000 * H * W / numThreads);
    }

    constexpr int kurageMaxSize = 5;

    DolphinSolver::verbose = verbose;
    KurageSolver::verbose = verbose;

    if(H == 2 && W == 2){
        
        // 普通に kurage
        KurageSolver kurage(problem);
        kurage.numThreads = numThreads;

        kurage.onCreatedAnswer = [&](const Answer& answer){
            Answer tmp = answer;
            tmp.optimize();
            onCreatedAnswer(tmp);
        };

        kurage.solve(start, problem.selectionLimit);
    }
    else 
    {
        constexpr int h = H < kurageMaxSize ? H : kurageMaxSize;
        constexpr int w = W < kurageMaxSize ? W : kurageMaxSize;
        constexpr int y = (H - h) / 2;
        constexpr int x = (W - w) / 2;

        RestrictedSquaredManhattanFeature<H, W>::y = y;
        RestrictedSquaredManhattanFeature<H, W>::x = x;
        RestrictedSquaredManhattanFeature<H, W>::h = h;
        RestrictedSquaredManhattanFeature<H, W>::w = w;

        std::vector<AnswerTreeBoard<H, W>> first = {AnswerTreeBoard<H, W>(start)};

        // dolphin で解く
        DolphinSolver dolphin;
        dolphin.beamWidth = 10;

        util::StopWatch::start("dolphin");
        const AnswerTreeBoard<H, W> result = dolphin.compute(first, y, x, h, w);
        util::StopWatch::stop_last();

        first.clear();
        const Answer answer = result.buildAnswer();
        std::cout << "Dolphin = " << answer.size() << std::endl;

        // PlayBoard<h, w> に変換
        PlayBoard<h, w> board;
        extractBoard(result, board, y, x);

        // kurage で解く
        KurageSolver kurage2(problem);
        kurage2.numThreads = numThreads;
        kurage2.onCreatedAnswer = [&](const Answer& arg){
           Answer final_answer = answer;
           appendAnswer(final_answer, arg, y, x);
           final_answer.optimize();
           onCreatedAnswer(final_answer);
       };

       util::StopWatch::start("second kurage");
       kurage2.solve(board, problem.selectionLimit - 1);
       util::StopWatch::stop_last();
   }
}

template<>
void LizardSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board)
{
    branch(make_solve_caller(*this), board);
}

void LizardSolver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board));
}

} // end of namespace slide
