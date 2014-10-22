#ifndef SLIDE_PLAY_BOARD_UTILITY_HPP_
#define SLIDE_PLAY_BOARD_UTILITY_HPP_

#include "Direction.hpp"
#include "Point.hpp"

namespace slide
{

template<typename Base>
class PlayBoardUtility : public Base
{
public:
    using Base::Base;
    using Base::selected;

    /**************************************************************************
     * The move operations
     *************************************************************************/

    void moveUp() {
        Base::move(Direction::Up);
    }

    void moveRight() {
        Base::move(Direction::Right);
    }

    void moveDown() {
        Base::move(Direction::Down);
    }

    void moveLeft() {
        Base::move(Direction::Left);
    }

    void moveVertically(int dstY) {
        const int n = selected.y - dstY;
        rep(i, n)  moveUp();
        rep(i, -n) moveDown();
    }

    void moveHorizontally(int dstX) {
        const int n = selected.x - dstX;
        rep(i, n)  moveLeft();
        rep(i, -n) moveRight();
    }

    /**************************************************************************
     * The select operations
     *************************************************************************/

    void select(Point newSelect) {
        Base::select(newSelect);
    }

    void select(int y, int x) {
        Base::select(Point(y, x));
    }
};

} // end of namespace slide

#endif
