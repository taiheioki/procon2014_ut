#ifndef SLIDE_ADMISSIBLE_BOARD_HPP_
#define SLIDE_ADMISSIBLE_BOARD_HPP_

#include "CostFeature.hpp"
#include "ManhattanFeature.hpp"
#include "ParityFeature.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class CMPFeatures : public CostFeature, public ManhattanFeature {};

template<int H, int W = H>
class AdmissibleBoardBase : public PlayBoardBase<H, W>, public CMPFeatures
{
private:
    ParityFeature<H, W> parity;

public:
    int lowerBound;

    using PlayBoardBase<H, W>::isSelected;
    using PlayBoardBase<H, W>::selected;
    using PlayBoardBase<H, W>::width;
    using PlayBoardBase<H, W>::height;

    AdmissibleBoardBase() = default;
    AdmissibleBoardBase(const PlayBoard<H, W>& board, int selectionLimit) {
        init(board, selectionLimit);
    }

    void init(const PlayBoard<H, W>& board, int selectionLimit)
    {
        PlayBoardBase<H, W>::operator=(board);
        CostFeature::init(selectionLimit);
        ManhattanFeature::init(*this);
        parity.init(*this);
        lowerBound = computeLowerBound();
    }

    constexpr bool isFinished() const
    {
        return manhattan == 0;
    }

    /**************************************************************************
     * Heuristic
     *************************************************************************/

    int computeHeuristic() const
    {
        if(isPrunnable()){
            return 1 << 29;
        }

        // Manhattan 1
        const int m1 = swappingCost * ((selManhattan + manhattan + 1) >> 1) + (isSelected() && !parity() ? 0 : selectionCost);

        // Manhattan 2
        constexpr int manhattan2Table[] = {0, 2, 4, 7, 10, 13, 16, 20, 24, 28, 32, 36, 40, 45, 50, 55, 60};
        const int m2 = swappingCost * manhattan
            - std::max(0, ((width()+height())*swappingCost - selectionCost) * selectionLimit - manhattan2Table[selectionLimit]*swappingCost);

        int h = std::max(m1, m2);
        return h;
    }

    int computeLowerBound() const {
        return cost + computeHeuristic();
    }

    bool isPrunnable() const {
        return selectionLimit == 0 && parity();
    }

    /**************************************************************************
     * Operations
     *************************************************************************/

    void move(Direction dir)
    {
        PlayBoardBase<H, W>::move(dir);
        CostFeature::move();
        ManhattanFeature::move(*this, dir);
        parity.move();
        lowerBound = computeLowerBound();
    }

    void select(Point newSelect)
    {
        PlayBoardBase<H, W>::select(newSelect);
        CostFeature::select();
        ManhattanFeature::select(*this);
        parity.select(selManhattan);
        lowerBound = computeLowerBound();
    }
};

template<int H, int W = H>
using AdmissibleBoard = PlayBoardUtility<AdmissibleBoardBase<H, W>>;

} // end of namespace slide

#endif
