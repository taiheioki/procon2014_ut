#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>

#include "PlayBoard.hpp"
#include "ExactSolver.hpp"
#include "branch.hpp"

namespace slide
{

template<int H, int W>
int ExactSolver::startDFS(const AdmissibleBoard<H, W>& board, int lb)
{
    if(board.isFinished()){
        return -1; // -1 means the answer was found
    }

    ++visitedNode;

    int min = 1 << 29;

    // select
    rep(i, board.height()) rep(j, board.width()) {

        AdmissibleBoard<H, W> next = board;
        next.select(i, j);
        if(next.lowerBound > lb){
            min = std::min(min, next.lowerBound);
            continue;
        }

        const int ret = DFS(next, lb, Move(Point(i, j)));

        if(ret == -1){
            answer.emplace_back(Point(i, j));
            return -1;
        }

        min = std::min(min, ret);
    }

    return min;   
}

template<int H, int W>
int ExactSolver::DFS(AdmissibleBoard<H, W>& board, int lb, Move preMove)
{
    if(board.isFinished()){
        return -1; // -1 means the answer was found
    }

    ++visitedNode;

    int min = 1 << 29;

    // move
    rep(k, 4){
        const Direction dir(k);
        if(!board.isValidMove(dir) || (!preMove.isSelection && preMove.getDirection() == dir.opposite())){
            continue;
        }

        AdmissibleBoard<H, W> next = board;
        next.move(dir);
        if(next.lowerBound > lb){
            min = std::min(min, next.lowerBound);
            continue;
        }

        const int ret = DFS(next, lb, Move(dir));

        if(ret == -1){
            answer.emplace_back(dir);
            return -1;
        }

        min = std::min(min, ret);
    }

    // select
    if(board.selectionLimit > 0 && !preMove.isSelection){
    	const CMPFeatures cmp = board;
        const Point preSelected = board.selected;

        rep(i, board.height()) rep(j, board.width()) {
            if(Point(i, j) == preSelected){
                continue;
            }

            board.select(i, j);
            if(board.lowerBound > lb){
                min = std::min(min, board.lowerBound);
                board.CMPFeatures::operator=(cmp);
                continue;
            }

            const int ret = DFS(board, lb, Move(Point(i, j)));
            board.CMPFeatures::operator=(cmp);

            if(ret == -1){
                answer.emplace_back(Point(i, j));
                return -1;
            }

            min = std::min(min, ret);
        }

        board.selected = preSelected;
    }

    return min;
}

template<int H, int W>
void ExactSolver::IDAstar(const PlayBoard<H, W>& board)
{
    AdmissibleBoard<H, W> start(board, problem.selectionLimit);
    int lb = start.lowerBound;

    for(;;){
        std::cerr << lb << " ";
        const int next = startDFS(start, lb);

        if(next == -1){
            break;
        }
        else{
            lb = next;
        }
    }

    std::reverse(answer.begin(), answer.end());
}

template<int H, int W>
void ExactSolver::solve(const PlayBoard<H, W>& board)
{
    CostFeature::setCosts(problem.swappingCost, problem.selectionCost);
    visitedNode = 0;

    IDAstar(board);
    std::cerr << "visited " << visitedNode << " nodes!" << std::endl;

    answer.optimize();
    BOOST_ASSERT(problem.check(answer));

    onCreatedAnswer(answer);
}

template<>
void ExactSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board)
{
    branch(make_solve_caller(*this), board);
}

void ExactSolver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board));
}

} // end of namespace slide
