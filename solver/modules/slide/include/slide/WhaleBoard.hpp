#ifndef SLIDE_WHALE_BOARD_HPP_
#define SLIDE_WHALE_BOARD_HPP_

#include "AnswerTreeFeature.hpp"
#include "KurageHashFeature.hpp"
#include "LinearConflictFeature.hpp"
#include "ManhattanFeature.hpp"
#include "PlayBoard.hpp"
#include "BitBoard.hpp"
#include "SquaredManhattanFeature.hpp"
#include "VarianceFeature.hpp"

#include "util/StopWatch.hpp"
#include "util/sqrt_approx.hpp"

namespace slide
{

template<int H, int W = H>
class WhaleBoard : public PlayBoardBase<H, W>, public ManhattanFeature, public AnswerTreeFeature
{
private:
    LinearConflictFeature<H, W> linearConflict;
    RestrictedSquaredManhattanFeature<H, W> squaredManhattan;

public:
    KurageHashFeature<H, W> hash;

    Move preMove;
    float score;
    int firstManhattan;

    using PlayBoardBase<H, W>::height;
    using PlayBoardBase<H, W>::width;
    using PlayBoardBase<H, W>::at;
    using PlayBoardBase<H, W>::selected;
    using PlayBoardBase<H, W>::isAligned;
    using PlayBoardBase<H, W>::arrayH;
    using PlayBoardBase<H, W>::arrayW;

    WhaleBoard() = default;

    WhaleBoard(const PlayBoard<H, W>& board)
    {
        init(board);
    }

    void init(const PlayBoard<H, W>& board)
    {
        PlayBoardBase<H, W>::operator=(board);
        ManhattanFeature::init(*this);
        linearConflict.init(*this);
        squaredManhattan.init(*this);
        AnswerTreeFeature::init();
        hash.init(*this);
        firstManhattan = manhattan;
        preMove = Move(Direction::Up);
    }

    constexpr bool isFinished() const
    {
        return manhattan == 0;
    }

    /**************************************************************************
     * Evaluation
     *************************************************************************/

    // more less, more better
    float evaluate(int thread = util::ThreadIndexManager::getLocalId()) const
    {
        float s = 0.0f;
        s += 10 * manhattan;
        s += 20 * linearConflict();
        s += 60 * manhattan / (firstManhattan + 1) * util::Random::nextReal(thread);
        s += 200 * squaredManhattan();
        return s;
    }

    /**************************************************************************
     * Operations
     *************************************************************************/

    void move(Direction dir, int thread = util::ThreadIndexManager::getLocalId())
    {
        PlayBoardBase<H, W>::move(dir);
        ManhattanFeature::move(*this, dir);
        linearConflict.move(*this, dir);
        squaredManhattan.move(*this, dir);
        AnswerTreeFeature::move(dir, thread);
        hash.move(*this, dir);

        preMove = Move(dir);
        score = evaluate(thread);
    }

    void select(Point newSelect, int thread = util::ThreadIndexManager::getLocalId())
    {
        const Point preSelect = selected;
        
        PlayBoardBase<H, W>::select(newSelect);
        ManhattanFeature::select(*this);
        linearConflict.select(*this, preSelect);
        squaredManhattan.select(*this, preSelect);
        AnswerTreeFeature::select(newSelect, thread);
        hash.select(*this);

        preMove = Move(newSelect);
        score = evaluate(thread);
    }
};

} // end of namespace slide

#endif
