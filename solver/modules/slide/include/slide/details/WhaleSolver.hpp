#ifndef SLIDE_DETAILS_WHALE_SOLVER_HPP_
#define SLIDE_DETAILS_WHALE_SOLVER_HPP_

#include <utility>

#include <tbb/parallel_for_each.h>
#include <tbb/parallel_sort.h>
#include <tbb/task_scheduler_init.h>

#include "../ParityFeature.hpp"
#include "../WhaleSolver.hpp"
#include "util/dense_hash_set.hpp"
#include "util/SpinMutex.hpp"
#include "util/ThreadIndexManager.hpp"

namespace slide
{

template<int H, int W>
std::vector<AnswerTreeBoard<H, W>> WhaleSolver::decreaseManhattan(
    const PlayBoard<H, W>& start, int selectionLimit, int y, int x, int h, int w
){
    if(numThreads == -1){
        numThreads = tbb::task_scheduler_init::default_num_threads();
    }

    if(KurageHashFeature<H, W>::table == nullptr){
        KurageHashFeature<H, W>::table.reset(new ZobristTable<H, W>(start.height(), start.width()));
    }

    using BoardsArray = std::vector<std::vector<WhaleBoard<H, W>>>;
    BoardsArray boards(numThreads);
    BoardsArray nextBoards(numThreads);
    rep(i, numThreads){
        nextBoards[i].reserve(totalBeamWidth * 4 / numThreads);
    }

    // 最初の選択
    {
        const WhaleBoard<H, W> kurage(start);

        if(kurage.isSelected()){
            // already selected
            BOOST_ASSERT(Point(start(start.selected)).isIn(y, x, h, w));
            boards[0].push_back(kurage);
        }
        else{
            // the first select

            // パリティ計算
            ParityFeature<H, W> parity(start);

            int cnt = 0;
            rep(i, kurage.height()) rep(j, kurage.width()){
                if(Point(kurage(i, j)).isIn(y+1, x+1, h-2, w-2)){
                    const bool p = parity() ^ (((Point(i, j) - Point(start(i, j))).l1norm()) & 1);
                    if(!p){
                        boards[0].emplace_back(kurage);
                        boards[0][boards[0].size()-1].select(Point(i, j));
                        ++cnt;
                    }
                }
            }

            if(cnt == 0){
                rep(i, kurage.height()) rep(j, kurage.width()){
                    if(Point(kurage(i, j)).isIn(y+1, x+1, h-2, w-2)){
                        boards[0].emplace_back(kurage);
                        boards[0][boards[0].size()-1].select(Point(i, j));
                    }
                }
            }

            --selectionLimit;
        }
    }

    // const int firstManhattan = kurage.manhattan;
    // double remainRatio = 1.0;

    // 登場したハッシュ値のリスト
    util::dense_hash_set<ull> visited(0ull);

    // インデックス
    std::vector<std::pair<float, uint>> indices;
    indices.reserve(totalBeamWidth * 4);
    int indices_shift;
    for(indices_shift = 0; (1 << indices_shift) < numThreads; ++indices_shift);

    bool finished = false;
    int reliability = 10;
    int preMinManhattan = 1 << 29;

    // r は交換回数
    for(int r=0;; ++r){

        // board 達をソート
        util::StopWatch::start("sorting");
        
        indices.clear();
        rep(i, numThreads){
            rep(j, boards[i].size()){
                indices.push_back({boards[i][j].score, (uint(j)<<uint(indices_shift)) | uint(i)});
            }
        }

        if(indices.size() > totalBeamWidth){
            tbb::parallel_sort(indices.begin(), indices.end());
            indices.erase(indices.begin() + totalBeamWidth, indices.end());
        }
        util::StopWatch::stop("sorting");

        // visited フラグを追加
        util::StopWatch::start("visited");
        for(const std::pair<float, uint>& id : indices){
            const ull hash = boards[id.second & ((1<<indices_shift)-1)][id.second >> indices_shift].hash();
            visited.insert(hash);
        }
        util::StopWatch::stop("visited");

        // 統計情報を計算
        int minManhattan = 1 << 29;
        for(const std::pair<float, uint>& id : indices){
            const WhaleBoard<H, W>& board = boards[id.second & ((1<<indices_shift)-1)][id.second >> indices_shift];
            minManhattan = std::min(minManhattan, int(board.manhattan));
        }

        reliability += preMinManhattan <= minManhattan ? -2 : 1;
        reliability = std::min(10, reliability);
        preMinManhattan = minManhattan;

        if(reliability <= 0){
            std::vector<AnswerTreeBoard<H, W>> ret;
            ret.reserve(indices.size());
            for(const std::pair<float, uint>& id : indices){
                const WhaleBoard<H, W>& preboard = boards[id.second & ((1<<indices_shift)-1)][id.second >> indices_shift];
                AnswerTreeBoard<H, W> board(preboard);
                board.selected = preboard.selected;
                board.swappingCount = r;
                board.index = preboard.index;
                board.thread = preboard.thread;
                ret.push_back(std::move(board));
            }
            std::cout << "minManhattan = " << minManhattan << std::endl;
            std::cout << "whale answer size = " << r << std::endl;
            return ret;
        }

        util::SpinMutex scoreMutex;

        // ！並列！
        util::StopWatch::start("parallel");

        tbb::parallel_for_each(indices.begin(), indices.end(), [
            this,
            &boards, &visited, &indices, indices_shift, // read-only
            &nextBoards, &scoreMutex, &finished         // read and write
        ]
        (std::pair<float, uint> id){
            const WhaleBoard<H, W>& board = boards[id.second & ((1<<indices_shift)-1)][id.second >> indices_shift];
            const int thread = util::ThreadIndexManager::getLocalId();

            // move
            int added = 0;
            const int preSize = nextBoards[thread].size();

            rep(k, 4){
                const Direction dir(k);
                if(!board.isValidMove(dir) || (!board.preMove.isSelection && board.preMove.getDirection() == dir.opposite())){
                    continue;
                }

                const int tmpIndex = preSize + added;
                nextBoards[thread].push_back(board);
                nextBoards[thread][tmpIndex].move(dir, thread);

                // check whether the finished or not
                if(nextBoards[thread][tmpIndex].isFinished()){
                    std::lock_guard<util::SpinMutex> lock(scoreMutex);
                    if(!finished){
                        Answer answer = nextBoards[thread][tmpIndex].buildAnswer();
                        answer.optimize();
                        onCreatedAnswer(std::move(answer));
                        finished = true;
                    }
                    return;
                }

                if(visited.count(nextBoards[thread][tmpIndex].hash())){
                    nextBoards[thread].pop_back();
                }
                else{
                    ++added;
                }
            }

            // add to next
            if(added == 3){
                if(nextBoards[thread][preSize].score < nextBoards[thread][preSize+1].score){
                    if(nextBoards[thread][preSize+1].score > nextBoards[thread][preSize+2].score){
                        // remain 0 and 2
                        nextBoards[thread][preSize+1] = std::move(nextBoards[thread][preSize+2]);
                    }
                }
                else if(nextBoards[thread][preSize].score > nextBoards[thread][preSize+2].score){
                    // remain 1 and 2
                    nextBoards[thread][preSize] = std::move(nextBoards[thread][preSize+2]);
                }
                nextBoards[thread].pop_back();
            }
        });

        util::StopWatch::stop("parallel");

        // swap and clear
        rep(i, numThreads){
            boards[i].clear();
            boards[i].swap(nextBoards[i]);
        }

        verbose && std::cerr << minManhattan << ' ';
    }
}

} // end of namespace slide

#endif
