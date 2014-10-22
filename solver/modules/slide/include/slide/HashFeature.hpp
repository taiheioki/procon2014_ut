#ifndef SLIDE_HASH_FEATURE_HPP_
#define SLIDE_HASH_FEATURE_HPP_

#include <memory>

#include "PlayBoard.hpp"
#include "ZobristTable.hpp"

namespace slide
{

template<int H, int W>
class HashFeature
{
private:
    ull selHash;

public:
    const ZobristTable<H, W>* table;
    ull hash;

    HashFeature() = default;

    HashFeature(const PlayBoardBase<H, W>& board, int selectionLimit, const ZobristTable<H, W>* table) {
        init(board, selectionLimit, table);
    }

    void init(const PlayBoardBase<H, W>& board, int selectionLimit, const ZobristTable<H, W>* table)
    {
        this->table = table;
        hash = 0ull;

        rep(i, board.height()) rep(j, board.width()) {
            hash ^= table->look(board(i, j), Point(i, j));
        }

        if(board.isSelected()){
            selHash = table->lookSelected(board(board.selected)) ^ selectionLimit;
            hash ^= selHash;
        }
        else{
            selHash = 0ull;
        }
    }

    void move(const PlayBoardBase<H, W>& board, Direction dir)
    {
        const Point src1 = board.selected - Point::delta(dir);
        const Point src2 = board.selected;
        const uchar id1  = board(src2);
        const uchar id2  = board(src1);

        hash ^= table->look(id1, src1) ^ table->look(id1, src2) ^ table->look(id2, src1) ^ table->look(id2, src2);
    }

    void select(const PlayBoardBase<H, W>& board, int selectionLimit)
    {
        hash ^= selHash;
        selHash = table->lookSelected(board(board.selected)) ^ selectionLimit; 
        hash ^= selHash;
    }

    void unsetSelection()
    {
        hash ^= selHash;
        selHash = 0ull;
    }
};

} // end of namespace slide

#endif
