#ifndef SLIDE_KURAGE_HASH_FEATURE_HPP_
#define SLIDE_KURAGE_HASH_FEATURE_HPP_

#include <memory>

#include "PlayBoard.hpp"
#include "ZobristTable.hpp"

namespace slide
{

template<int H, int W>
class KurageHashFeature
{
private:
    ull selHash;
    ull hash;

public:
    static std::unique_ptr<const ZobristTable<H, W>> table;

    KurageHashFeature() = default;

    KurageHashFeature(const PlayBoardBase<H, W>& board) {
        init(board);
    }

    void init(const PlayBoardBase<H, W>& board)
    {
        hash = 0ull;

        rep(i, board.height()) rep(j, board.width()) {
            hash ^= table->look(board(i, j), Point(i, j));
        }

        if(board.isSelected()){
            selHash = table->lookSelected(board(board.selected));
            hash ^= selHash;
        }
        else{
            selHash = 0ull;
        }
    }

    ull operator()() const
    {
        return hash;
    }

    void move(const PlayBoardBase<H, W>& board, Direction dir)
    {
        const Point src1 = board.selected - Point::delta(dir);
        const Point src2 = board.selected;
        const uchar id1  = board(src2);
        const uchar id2  = board(src1);

        hash ^= table->look(id1, src1) ^ table->look(id1, src2) ^ table->look(id2, src1) ^ table->look(id2, src2);
    }

    void select(const PlayBoardBase<H, W>& board)
    {
        hash ^= selHash;
        selHash = table->lookSelected(board(board.selected));
        hash ^= selHash;
    }
};

template<int H, int W>
std::unique_ptr<const ZobristTable<H, W>> KurageHashFeature<H, W>::table = nullptr;

} // end of namespace slide

#endif
