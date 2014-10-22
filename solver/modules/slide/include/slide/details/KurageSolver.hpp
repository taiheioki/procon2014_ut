#ifndef SLIDE_DETAILS_KURAGE_SOLVER_HPP_
#define SLIDE_DETAILS_KURAGE_SOLVER_HPP_

#include <tbb/parallel_for_each.h>
#include <tbb/parallel_sort.h>
#include <tbb/task_scheduler_init.h>

#include "../KurageSolver.hpp"
#include "util/dense_hash_set.hpp"
#include "util/SpinMutex.hpp"
#include "util/ThreadIndexManager.hpp"

namespace slide
{

inline void KurageSolver::determineBeamWidth(
    double remainRatio,
    int h,
    int w,
    const std::vector<int>& reliability,
    std::vector<int>& width,
    int preMinLayer,
    int& minLayer,
    int& maxLayer
)
{
    if(h*w <= 36){
        // 6x6 以下だと常に均等に割り振る
        const int N = maxLayer - minLayer + 1;
        reps(i, minLayer, maxLayer + 1){
            width[i] = totalBeamWidth / N;
        }
    }
    else{
        while(maxLayer > preMinLayer + 1 && reliability[maxLayer] < 0){
            --maxLayer;
        }

        const double x = 1 - (1-remainRatio)*(1-remainRatio);
        minLayer = std::max(minLayer, int(x * (maxLayer + 2) - 1e-10));
        minLayer = std::min(minLayer, maxLayer);

        const int N = maxLayer - minLayer + 1;
        reps(i, minLayer, maxLayer + 1){
            width[i] = totalBeamWidth / N;
        }
    }
}

template<int H, int W>
bool KurageSolver::beamSearch(const PlayBoard<H, W>& start, int selectionLimit)
{
    // 並列化数の設定
    if(numThreads == -1){
        numThreads = tbb::task_scheduler_init::default_num_threads();
    }
    tbb::task_scheduler_init init(numThreads);
    util::ThreadIndexManager::clear();

    // テーブルの初期化
    if(KurageHashFeature<H, W>::table == nullptr){
        KurageHashFeature<H, W>::table.reset(new ZobristTable<H, W>(start.height(), start.width()));
    }

    const KurageBoard<H, W> kurage(start, selectionLimit);

    // 初めから終わってた場合
    if(kurage.isFinished()){
        onCreatedAnswer({});
        return true;
    }

    const int nLayer = start.isSelected() ? selectionLimit + 1 : selectionLimit;

    // boards と nextBoards の初期化
    using BoardsArray = std::vector<std::vector<std::vector<KurageBoard<H, W>>>>;
    BoardsArray boards(numThreads);
    BoardsArray nextBoards(numThreads);
    rep(i, numThreads){
        boards[i].resize(nLayer);
        nextBoards[i].resize(nLayer);

        rep(j, nLayer){
            boards[i][j].reserve(totalBeamWidth * 4 / (numThreads * nLayer));
            nextBoards[i][j].reserve(totalBeamWidth * 4 / (numThreads * nLayer));
        }
    }

    // 最初の選択
    if(kurage.isSelected()){
        // already selected
        boards[0][nLayer - 1].push_back(kurage);
    }
    else{
        // the first select
        rep(i, kurage.height()) rep(j, kurage.width()){
            boards[0][nLayer - 1].emplace_back(kurage);
            boards[0][nLayer - 1][boards[0][nLayer-1].size()-1].select(Point(i, j), nLayer-1);
        }
    }

    const int firstManhattan = kurage.manhattan;
    double remainRatio = 1.0;

    // ハッシュ値が登場した，最も多い残選択回数
    util::dense_hash_map<ull, int> visited(0ull);

    // 現在見つかった，最も良い解（から1回分の交換コストを引いたもの）
    int bestScore = 1 << 29;
    AnswerTreeFeature bestAnswer;

    // マンハッタン距離の減少量
    std::vector<int> preManhattan(nLayer, 1 << 29);
    std::vector<int> reliability(nLayer, 20);
    std::vector<int> beamWidth(nLayer);

    // インデックス
    std::vector<std::pair<float, int>> indices;
    indices.reserve(totalBeamWidth * 4);
    int indices_shift;
    for(indices_shift = 0; (1 << indices_shift) < numThreads; ++indices_shift);

    // 用いるレイヤ番号の最小値と最大値
    int minLayer = 0;
    int maxLayer = nLayer - 1;
    int preMinLayer = 0;

    // r は選択回数と交換回数の和
    for(int r=0;; ++r){
        int allMinManhattan = 1 << 29;

        verbose && std::cerr << r << ": ";

        // レイヤ番号の最小値・最大値を更新
        {
            // 既に解答が作られている場合
            if(bestScore != 1 << 29){
                const int den = problem.selectionCost - problem.swappingCost;
                if(den > 0){
                    minLayer = ((r-nLayer)*problem.swappingCost + nLayer*problem.selectionCost - bestScore + den - 1)/den;
                    if(minLayer > maxLayer){
                        break;
                    }
                }
                else{
                    break;
                }
            }
            else{
                minLayer = 0;
            }

            // ビーム幅を決定
            determineBeamWidth(remainRatio, start.height(), start.width(), reliability, beamWidth, preMinLayer, minLayer, maxLayer);
        }

        for(int layer=maxLayer; layer>=minLayer; --layer){

            {
                int sum = 0;
                rep(i, numThreads){
                    sum += boards[i][layer].size();
                }
                if(sum == 0){
                    rep(i, numThreads){
                        boards[i][layer].swap(nextBoards[i][layer]);
                    }
                    continue;
                }
            }
            preMinLayer = layer;

            // board 達をソート
            util::StopWatch::start("sorting");

            indices.clear();
            rep(i, numThreads){
                rep(j, boards[i][layer].size()){
                    indices.push_back({boards[i][layer][j].score, (uint(j)<<uint(indices_shift)) | uint(i)});
                }
            }

            if(indices.size() > beamWidth[layer]){
                tbb::parallel_sort(indices.begin(), indices.end());
                indices.erase(indices.begin() + beamWidth[layer], indices.end());
            }

            util::StopWatch::stop_last();

            // visited フラグを追加
            util::StopWatch::start("visited");

            for(const std::pair<float, uint>& id : indices){
                const ull hash = boards[id.second & ((1<<indices_shift)-1)][layer][id.second >> indices_shift].hash();
                util::dense_hash_map<ull, int>::iterator itr = visited.find(hash);
                if(itr != visited.end()){
                    itr->second = std::max(itr->second, layer);
                }
                else{
                    visited.insert({hash, layer});
                }
            }

            util::StopWatch::stop_last();

            // 統計情報を計算
            int minManhattan = 1 << 29;
            int preSelectedNum = 0;
            for(const std::pair<float, uint>& id : indices){
                const KurageBoard<H, W>&board = boards[id.second & ((1<<indices_shift)-1)][layer][id.second >> indices_shift];
                minManhattan = std::min(minManhattan, int(board.manhattan));
                preSelectedNum += board.preMove.isSelection;
            }

            util::StopWatch::start("parallel");

            util::SpinMutex scoreMutex;

            // ！並列！
            tbb::parallel_for_each(indices.begin(), indices.end(), [
                this,
                &boards, nLayer, layer, r, minLayer, &visited, &indices, indices_shift, // read-only
                &nextBoards, &bestScore, &scoreMutex, &bestAnswer                       // read and write
            ]
            (std::pair<float, uint> id){
                const KurageBoard<H, W>& board = boards[id.second & ((1<<indices_shift)-1)][layer][id.second >> indices_shift];
                const int thread = util::ThreadIndexManager::getLocalId();

                // move
                rep(k, 4){
                    const Direction dir(k);
                    if(!board.isValidMove(dir) || (!board.preMove.isSelection && board.preMove.getDirection() == dir.opposite())){
                        continue;
                    }

                    nextBoards[thread][layer].push_back(board);
                    KurageBoard<H, W>& next = nextBoards[thread][layer][nextBoards[thread][layer].size() - 1];
                    next.move(dir, layer);

                    // check whether the finished or not
                    if(next.isFinished()){
                        const int score = (r - (nLayer - layer)) * problem.swappingCost + (nLayer - layer) * problem.selectionCost;
                        std::lock_guard<util::SpinMutex> lock(scoreMutex);
                        if(score < bestScore){
                            bestScore = score;
                            bestAnswer = next;
                        }
                    }

                    const util::dense_hash_map<ull, int>::const_iterator itr = visited.find(next.hash());
                    if(itr != visited.end() && itr->second >= layer){
                        nextBoards[thread][layer].pop_back();
                    }
                }

                // select
                if(layer > minLayer && !board.preMove.isSelection){
                    const int maxNeighbor = board.area() <= 36 || layer == minLayer + 1 ? 1 : 0;
                    if((board.selected - Point(board(board.selected))).l1norm() <= maxNeighbor){
                        rep(i, board.height()) rep(j, board.width()){
                            if(board.selected == Point(i, j) || board.isAligned(i, j)){
                                continue;
                            }

                            nextBoards[thread][layer-1].push_back(board);
                            KurageBoard<H, W>& next = nextBoards[thread][layer-1][nextBoards[thread][layer-1].size() - 1];
                            next.select(Point(i, j), layer);

                            if(next.isPrunnable(layer-1)){
                                nextBoards[thread][layer-1].pop_back();
                            }
                            else{
                                const util::dense_hash_map<ull, int>::const_iterator itr = visited.find(next.hash());
                                if(itr != visited.end() && itr->second >= layer-1){
                                    nextBoards[thread][layer-1].pop_back();
                                }
                            }
                        }
                    }
                }
            });
            util::StopWatch::stop_last();

            // swap and clear
            rep(i, numThreads){
                boards[i][layer].clear();
                boards[i][layer].swap(nextBoards[i][layer]);
            }

            // update reliability
            reliability[layer] += preManhattan[layer] <= minManhattan ? -2 : 1;
            preManhattan[layer] = minManhattan;
            if(preSelectedNum > 0){
                const int add = 2 + 5 * preSelectedNum / indices.size();
                reliability[layer] += add;
                if(layer < maxLayer){
                    reliability[layer + 1] += add;
                }
            }
            reliability[layer] = std::min(reliability[layer], 20);

            verbose && std::cerr << '\t' << layer << ":"
                << (boost::format("%4d(%3d/%4d,%3d)") % minManhattan % preSelectedNum % indices.size() % reliability[layer]);
            allMinManhattan = std::min(allMinManhattan, minManhattan);
        }

        if(reliability[preMinLayer] < -5 * (kurage.height() + kurage.width())){
            break;
        }

        remainRatio = double(allMinManhattan) / firstManhattan;
        verbose && std::cerr << std::endl;
    }

    if(bestScore != 1 << 29){
        onCreatedAnswer(bestAnswer.buildAnswer());
        return true;
    }

    return false;
}

template<int H, int W>
inline void KurageSolver::solve(const PlayBoard<H, W>& board, int selectionLimit)
{
    visitedNode = 0;
    while(!beamSearch(board, selectionLimit) && retry){
        std::cerr << '!';
        AnswerLinearTree::nodes.clear();
    }
    verbose && std::cerr << "visited " << visitedNode << " nodes!" << std::endl;
}

template<>
inline void KurageSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board, int selectionLimit)
{
    branch(make_solve_caller(*this), board, selectionLimit);
}

} // end of namespace slide

#endif
