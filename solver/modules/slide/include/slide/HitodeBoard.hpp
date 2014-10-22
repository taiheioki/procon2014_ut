#ifndef SLIDE_HITODE_BOARD_HPP_
#define SLIDE_HITODE_BOARD_HPP_

#include <boost/optional.hpp>

#include "AnswerTreeFeature.hpp"
#include "HashFeature.hpp"
#include "AdmissibleBoard.hpp"

namespace slide
{

template<int H, int W = H>
class HitodeBoardBase : public AdmissibleBoard<H, W>, public HashFeature<H, W>
{
public:
	using AdmissibleBoard<H, W>::selectionLimit;
    using HashFeature<H, W>::hash;

    ull parentHash;
    Move preMove;

    HitodeBoardBase() = default;
    HitodeBoardBase(const PlayBoard<H, W>& board, int selectionLimit, const ZobristTable<H, W>* table) {
        init(board, selectionLimit, table);
    }

    void init(const PlayBoard<H, W>& board, int selectionLimit, const ZobristTable<H, W>* table)
    {
        AdmissibleBoard<H, W>::init(board, selectionLimit);
        HashFeature<H, W>::init(*this, selectionLimit, table);
        parentHash = 0ull;
        preMove = Move(Direction::Up);
    }

    bool operator< (const HitodeBoardBase& board) const {
        return AdmissibleBoard<H, W>::lowerBound > board.lowerBound;
    }

    /**************************************************************************
     * Operations
     *************************************************************************/

    void move(Direction dir)
    {
        parentHash = hash;
        AdmissibleBoard<H, W>::move(dir);
        HashFeature<H, W>::move(*this, dir);
        preMove = Move(dir);
    }

    void select(Point newSelect)
    {
        parentHash = hash;
        AdmissibleBoard<H, W>::select(newSelect);
        HashFeature<H, W>::select(*this, selectionLimit);
        preMove = Move(newSelect);
    }
};

template<int H, int W = H>
using HitodeBoard = PlayBoardUtility<HitodeBoardBase<H, W>>;

} // end of namespace slide

#endif
