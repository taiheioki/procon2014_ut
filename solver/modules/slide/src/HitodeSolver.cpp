#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "HitodeSolver.hpp"
#include "util/dense_hash_map.hpp"
#include "util/SpinMutex.hpp"

namespace slide
{

namespace
{

struct Data
{
    ull parent;
    uchar selectionLimit;
    Move preMove;
};

template<int H, int W>
class AstarWorker
{
public:
    util::SpinMutex mutex;

public:
    std::atomic_ullong metHash;
    util::dense_hash_map<ull, Data> visited;
    AstarWorker* other;
    int maxSelectionLimit;
    int threadId;

    explicit AstarWorker() : metHash(0ull), visited(0ull) {}

    void run(const HitodeBoard<H, W>& start);
    void operator()(const HitodeBoard<H, W>& start) {
        run(start);
    }

    Answer buildAnswer(ull start) const
    {
        Answer answer;

        while(start){
            answer.emplace_back(visited.at(start).preMove);
            start = visited.at(start).parent;
        }

        answer.pop_back();
        std::reverse(answer.begin(), answer.end());

        return answer;
    }
};

template<int H, int W>
void AstarWorker<H, W>::run(const HitodeBoard<H, W>& start)
{
    int max = 0;

    std::priority_queue<HitodeBoard<H, W>> Q;
    Q.push(start);

    while(!Q.empty()){

        // check finished
        if(other->metHash.load(std::memory_order_acquire)){
            return;
        }

        const HitodeBoard<H, W> board = Q.top();
        Q.pop();

        // check visited
        {
            if(!visited.count(board.hash)){
                {
                    std::lock_guard<util::SpinMutex> lock(mutex);

                    visited.insert(
                        std::pair<ull, Data>(board.hash,
                            {
                                board.parentHash,
                                static_cast<uchar>(board.selectionLimit),
                                board.preMove
                            }
                        )
                    );
                }
            }
            else if(visited.at(board.hash).selectionLimit >= board.selectionLimit){
                continue;
            }
            else{
                visited.at(board.hash) = {board.parentHash, static_cast<uchar>(board.selectionLimit), board.preMove};
            }
        }

        if(max < board.lowerBound){
            max = board.lowerBound;
            std::cerr << board.lowerBound << ", ";
        }

        // check whether finished or not
        bool isFinished;
        {
            std::lock_guard<util::SpinMutex> lock(other->mutex);
            isFinished = other->visited.count(board.hash)
                && board.selectionLimit + other->visited.at(board.hash).selectionLimit + 1 >= maxSelectionLimit;
        }

        if(isFinished){
            metHash.store(board.hash, std::memory_order_release);
            return;
        }

        // move
        rep(k, 4){
            const Direction dir = Direction(k);
            if(!board.isValidMove(dir) || (!board.preMove.isSelection && board.preMove.getDirection() == dir.opposite())){
                continue;
            }

            HitodeBoard<H, W> next = board;
            next.move(dir);
            if(!visited.count(next.hash) || visited.at(next.hash).selectionLimit < next.selectionLimit){
                Q.push(std::move(next));
            }
        }

        // select
        if(board.selectionLimit >= 1 && !board.preMove.isSelection){
            rep(i, board.height()) rep(j, board.width()){
                if(board.selected == Point(i, j)){
                    continue;
                }

                HitodeBoard<H, W> next = board;
                next.select(i, j);
                if(!next.isPrunnable() && (!visited.count(next.hash) || visited.at(next.hash).selectionLimit < next.selectionLimit)){
                    Q.push(std::move(next));
                }
            }
        }
    }
}

} // end of nonamed namespace

template<int H, int W>
Answer HitodeSolver::bidirectionalAstar(const PlayBoard<H, W>& board)
{
    // normal
    const ZobristTable<H, W> hashTable(board.height(), board.width());
    HitodeBoard<H, W> start1(board, problem.selectionLimit, &hashTable);

    // inverse board
    std::vector<uchar> table;    // now_id -> inv_id
    std::vector<uchar> invTable; // inv_id -> now_id
    const Board<H, W> invBoard = inverseBoard(board, table, invTable);

    // inverse hash table
    ZobristTable<H, W> invHashTable = hashTable;
    rep(i, board.height()) rep(j, board.width()){
        const uchar id = board.correctId(i, j);
        rep(y, board.height()) rep(x, board.width()){
            invHashTable.look(table[id], Point(y, x)) = hashTable.look(id, Point(y, x));
        }
        invHashTable.lookSelected(table[id]) = hashTable.lookSelected(id);
    }

    HitodeBoard<H, W> start2(invBoard, problem.selectionLimit, &invHashTable);

    // prepare worker
    AstarWorker<H, W> workers[2];
    workers[0].maxSelectionLimit = problem.selectionLimit;
    workers[1].maxSelectionLimit = problem.selectionLimit;
    workers[0].other = &workers[1];
    workers[1].other = &workers[0];
    workers[0].threadId = 0;
    workers[1].threadId = 1;

    // run thread
    std::thread threads[2] = {
        std::thread(std::ref(workers[0]), start1),
        std::thread(std::ref(workers[1]), start2)
    };

    threads[0].join();
    threads[1].join();

    const ull metHash = std::max(workers[0].metHash.load(), workers[1].metHash.load());
    visitedNode = workers[0].visited.size() + workers[1].visited.size();

    // construct answer
    Answer answer[2] = {workers[0].buildAnswer(metHash), workers[1].buildAnswer(metHash)};

    // combine answers
    {
        Answer tmp;

        for(int i=0; i<answer[1].size();){
            Point selected = answer[1][i].getSelected();
            ++i;
            for(; i<answer[1].size() && !answer[1][i].isSelection; ++i){
                const Direction dir = answer[1][i].getDirection();
                selected += Point::delta(dir);
                tmp.emplace_back(dir.opposite());
            }
            tmp.emplace_back(selected);
        }

        for(int i=tmp.size()-2; i>=0; --i){
            answer[0].push_back(tmp[i]);
        }
    }

    return answer[0];
}

template<int H, int W>
Board<H, W> HitodeSolver::inverseBoard(const Board<H, W>& board, std::vector<uchar>& table, std::vector<uchar>& invTable)
{
    table.resize(MAX_DIVISION_NUM * MAX_DIVISION_NUM);
    invTable.resize(MAX_DIVISION_NUM * MAX_DIVISION_NUM);

    rep(i, board.height()) rep(j, board.width()){
        table[board(i, j)] = board.correctId(i, j);
    }

    Board<H, W> ret(board.height(), board.width());
    rep(i, board.height()) rep(j, board.width()){
        const uchar id = board.correctId(i, j);
        ret(i, j) = table[id];
        invTable[ret(i, j)] = id;
    }

    return ret;
}

template<int H, int W>
void HitodeSolver::solve(const PlayBoard<H, W>& board)
{
    CostFeature::setCosts(problem.swappingCost, problem.selectionCost);
    visitedNode = 0;

    Answer answer = bidirectionalAstar(board);
    std::cerr << "visited " << visitedNode << " nodes!" << std::endl;

    answer.optimize();
    BOOST_ASSERT(problem.check(answer));
    onCreatedAnswer(answer);
}

template<>
void HitodeSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board)
{
    branch(make_solve_caller(*this), board);
}

void HitodeSolver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board));
}

};
