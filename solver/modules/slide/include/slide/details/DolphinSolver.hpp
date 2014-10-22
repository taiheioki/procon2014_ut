#ifndef SLIDE_DETAILS_DOLPHIN_SOLVER_HPP_
#define SLIDE_DETAILS_DOLPHIN_SOLVER_HPP_

#include <algorithm>
#include <vector>

#include "../DolphinBoard.hpp"
#include "../DolphinSolver.hpp"

#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/partitioner.h>
#include <tbb/tbb.h>

#include "util/StopWatch.hpp"
#include "util/SpinMutex.hpp"

namespace slide
{

template<int H, int W>
AnswerTreeBoard<H, W> DolphinSolver::compute(const std::vector<AnswerTreeBoard<H, W>>& start, int y, int x, int h, int w)
{
    using DolphinBoard = DolphinUrchinBoard<H, W>;
    //using DolphinBoard = DolphinOctopusBoard<H, W>;

    std::vector<DolphinBoard> boards;
    std::vector<DolphinBoard> nextBoards;

    if(start[0].isSelected()){
        boards.reserve(start.size());
        for(const AnswerTreeBoard<H, W>& board : start){
            BOOST_ASSERT(Point(board(board.selected)).isIn(y, x, h, w));
            DolphinBoard dolphin(board);
            dolphin.swappingCount = board.swappingCount;
            dolphin.index = board.index;
            dolphin.thread = board.thread;
            boards.push_back(std::move(dolphin));
        }
    }
    else{
        // [x, x+w)×[y, y+h) 内の全部の位置を初期位置としてセットする
        if(h == 2){
            rep(i, h) rep(j, w-2){
                DolphinBoard board(start[0]);
                board.select(board.find(board.correctId(i, j+x+1)));
                boards.push_back(board);
            }
        }
        else if(w == 2){
            rep(i, h-2) rep(j, w){
                DolphinBoard board(start[0]);
                board.select(board.find(board.correctId(i+y+1, j)));
                boards.push_back(board);
            }
        }
        else {
            rep(i, h-2) rep(j, w-2){
                DolphinBoard board(start[0]);
                board.select(board.find(board.correctId(i+y+1, j+x+1)));
                boards.push_back(board);
            }
        }
    }
    DolphinBoard returnAnswerBoard;
    returnAnswerBoard.swappingCount = 1 << 29;

    bool returnCheck = false;

    std::vector<std::pair<int, int>> boardIndices;
    util::SpinMutex scoreMutex;
    util::SpinMutex returnMutex;

    for(;;){
        int min = 1 << 29;
        int max = 0;
        if(verbose){
            for(const DolphinBoard& board : boards){
                min = std::min(min, board.swappingCount);
                max = std::max(max, board.swappingCount);
            }
        }

        //util::StopWatch::start("parallel for each");
        tbb::parallel_for_each(begin(boards), end(boards),
            [&returnMutex, &scoreMutex, &boardIndices, &returnCheck,
             &returnAnswerBoard, &nextBoards, y, x, h, w]
        (const DolphinBoard& board){

            std::vector<Point> cand, cand_tmp;
            board.getCandidates(cand_tmp);

            for(const Point& candidatePoint : cand_tmp){
                if(!candidatePoint.isIn(y, x, h, w)){
                    cand.emplace_back(candidatePoint);
                }
            }

            // check whether finished or not
            if(cand.empty()){
                std::lock_guard<util::SpinMutex> lock(returnMutex);
                if(returnAnswerBoard.swappingCount > board.swappingCount){
                    returnCheck = true;
                    returnAnswerBoard = board;
                }
            }

            // align piece
            for(const Point& candidatePoint : cand){
                DolphinBoard next = board;
                if(next.find(next.correctId(candidatePoint)) == next.selected)
                    continue;
                double score = next.align(candidatePoint);
                std::lock_guard<util::SpinMutex> lock(scoreMutex);
                boardIndices.emplace_back(score, boardIndices.size());
                nextBoards.push_back(std::move(next));
            }
        });
        //util::StopWatch::stop_last();

        if(returnCheck){
            verbose && std::cerr << '\n' << returnAnswerBoard << std::endl;
            return returnAnswerBoard;
        }

        // boardsのupdate , 5msec程度
        if(nextBoards.size() > beamWidth){
            std::partial_sort(boardIndices.begin(), boardIndices.begin() + beamWidth, boardIndices.end());
            boards.resize(beamWidth);
            rep(i, beamWidth){
                boards[i] = nextBoards[boardIndices[i].second];
            }
        }
        else{
            boards.swap(nextBoards);
        }

        nextBoards.clear();
        boardIndices.clear();

        verbose && std::cerr << min << '/' << max << ' ';
    }
}

} // end of namespace slide

#endif
