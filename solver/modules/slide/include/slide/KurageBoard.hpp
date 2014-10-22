#ifndef SLIDE_KURAGE_BOARD_HPP_
#define SLIDE_KURAGE_BOARD_HPP_

#include "AnswerTreeFeature.hpp"
#include "KurageHashFeature.hpp"
#include "LinearConflictFeature.hpp"
#include "ManhattanFeature.hpp"
#include "ParityFeature.hpp"
#include "PlayBoard.hpp"
#include "SquaredManhattanFeature.hpp"
#include "VarianceFeature.hpp"
#include "WeightedManhattanFeature.hpp"

#include "util/StopWatch.hpp"
#include "util/ThreadIndexManager.hpp"

namespace slide
{

template<int H, int W = H>
class KurageBoard : public PlayBoardBase<H, W>, public ManhattanFeature, public AnswerTreeFeature
{
private:
    LinearConflictFeature<H, W> linearConflict;
    ParityFeature<H, W> parity;
    VarianceFeature<H, W> variance;
    SquaredManhattanFeature<H, W> squaredManhattan;
    WeightedManhattanFeature<H, W> weightedManhattan;

public:
    KurageHashFeature<H, W> hash;

public:
    float score;
    Move preMove;
    int firstManhattan;

    using PlayBoardBase<H, W>::height;
    using PlayBoardBase<H, W>::width;
    using PlayBoardBase<H, W>::at;
    using PlayBoardBase<H, W>::selected;
    using PlayBoardBase<H, W>::isAligned;
    using PlayBoardBase<H, W>::arrayH;
    using PlayBoardBase<H, W>::arrayW;

    KurageBoard() = default;
    KurageBoard(const PlayBoard<H, W>& board, int selectionLimit) {
        init(board, selectionLimit);
    }

    void init(const PlayBoard<H, W>& board, int selectionLimit)
    {
        PlayBoardBase<H, W>::operator=(board);
        ManhattanFeature::init(*this);
        linearConflict.init(*this);
        parity.init(*this);
        AnswerTreeFeature::init();
        variance.init(*this);
        squaredManhattan.init(*this);
        weightedManhattan.init(*this);
        hash.init(*this);
        firstManhattan = manhattan;
        score = evaluate(selectionLimit);
        preMove = Move(Direction::Up);
    }

    constexpr bool isFinished() const {
        return manhattan == 0;
    }

    constexpr bool operator< (const KurageBoard& board) const {
        return score < board.score;
    }

    /**************************************************************************
     * Evaluation
     *************************************************************************/

    static float coefficients[5];

    // more less, more better
    float evaluate(int selectionLimit, int thread = util::ThreadIndexManager::getLocalId()) const
    {
        /*
        float score = 0.0f;
        score += coefficients[0] * manhattan;
        score += coefficients[1] * selManhattan;
        score += coefficients[2] * linearConflict();
        score += coefficients[3] * variance();
        //score += coefficients[4] * weightedManhattan();
        return score;
        */

        float s = 0.0f;
        s += 15 * manhattan;
        s += 15 * std::max(2.0f, 4.0f-selectionLimit/2.0f)*linearConflict();
        s += selManhattan;
        s += 60 * manhattan / (firstManhattan + 1) * util::Random::nextReal(thread);
        s += 30 * variance();
        s += 150 * squaredManhattan();

        if(H*2 <= W || W*2 <= H){
            s += 50 * weightedManhattan();
        }

        return s;
    }

    bool isPrunnable(int selectionLimit) const {
        return selectionLimit == 0 && parity();
    }

    /**************************************************************************
     * Operations
     *************************************************************************/

    void move(Direction dir, int selectionLimit, int thread = util::ThreadIndexManager::getLocalId())
    {
        PlayBoardBase<H, W>::move(dir);
        ManhattanFeature::move(*this, dir);
        linearConflict.move(*this, dir);
        variance.move(*this, dir);
        squaredManhattan.move(*this, dir);
        weightedManhattan.move(*this, dir);
        parity.move();
        AnswerTreeFeature::move(dir, thread);
        hash.move(*this, dir);

        score = evaluate(selectionLimit, thread);
        preMove = Move(dir);
    }

    void select(Point newSelect, int selectionLimit, int thread = util::ThreadIndexManager::getLocalId())
    {
        const Point preSelect = selected;
        PlayBoardBase<H, W>::select(newSelect);
        ManhattanFeature::select(*this);
        linearConflict.select(*this, preSelect);
        variance.select(*this, preSelect);
        squaredManhattan.select(*this, preSelect);
        weightedManhattan.select(*this, preSelect);
        parity.select(selManhattan);
        AnswerTreeFeature::select(newSelect, thread);
        hash.select(*this);

        score = evaluate(selectionLimit, thread);
        preMove = Move(newSelect);
    }
};

template<int H, int W>
float KurageBoard<H, W>::coefficients[5];

} // end of namespace slide

#endif
