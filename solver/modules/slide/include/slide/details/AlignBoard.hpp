#ifndef SLIDE_DETAILS_ALIGN_BOARD_HPP_
#define SLIDE_DETAILS_ALIGN_BOARD_HPP_

#include <vector>
#include <boost/format.hpp>

#include "../AlignBoard.hpp"
#include "util/color.hpp"
#include "util/StopWatch.hpp"

namespace slide
{

template<typename Base>
inline bool AlignBoard<Base>::extractSrcPiece(const Point src, const Point dst)
{
    if(fixed.countRowZero(src.y) == 1 || fixed.countColZero(src.x) == 1){
        rep(i, 4){
            const Direction dir(i);
            const Point tmp = src + Point::delta(dir);
            if(tmp.isIn(height(), width()) && fixed(tmp) && !isAligned(tmp)){
                fixed.reset(tmp);
                if(trajectPiece(src, dst)){
                    unfixCandidate(tmp);
                    return true;
                }
                fixed.set(tmp);
            }
        }
        return false;
    }
    rep(i, 4){
        const Direction dir(i);
        const Point tmp = src + Point::delta(dir);
        if(tmp.isIn(height(), width()) && fixed(tmp)){
            fixed.reset(tmp);
            if(trajectPiece(src, dst)){
                unfixCandidate(tmp);
                return true;
            }
            fixed.set(tmp);
        }
    }
    return false;
}

template<typename Base> 
inline bool AlignBoard<Base>::moveOneAnyDirection()
{
    rep(i, 4){
        const Direction dir(i);
        const Point to = selected + Point::delta(dir);
        if(to.isIn(height(), width()) && !fixed(to)){
            move(dir);
            return true;
        }
    }
    return false;
}

template<typename Base>
bool AlignBoard<Base>::align(const Point dst)
{
    // はじめにselectedが溝にいる場合は溝から出る
    if(countAdjacentFixedCell(selected) == 3){
        moveOneAnyDirection();
    }


    const int numRemain = std::min(width() - fixed.countRow(dst.y), height() - fixed.countCol(dst.x));
    if(numRemain > 2){
        const Point src = find(correctId(dst));
        if(src == dst){
            fixCandidate(dst);
            return true;
        }

        if(trajectPiece(src, dst)){
            fixCandidate(dst);
            return true;
        }
        else{
            // srcが溝に埋まっている場合はその隣接するマスのfixedを解除して,
            // trajectPieceできるか判断し, 成功したならtrueを返す.
            if(countAdjacentFixedCell(src) == 3){
                return extractSrcPiece(src, dst);
            }
        }
    }
    else if(numRemain == 2){
        bool flag;
        Point src;
        if(fixed.countRowZero(dst.y) < fixed.countColZero(dst.x)){
            if(!Point(dst.y, dst.x+1).isIn(height(), width()) ||
                (Point(dst.y, dst.x+1).isIn(height(), width()) && fixed(dst.y, dst.x+1))){
                src = find( (dst+Point(0, -1)).toInt() );
                if( src == selected ) return false;
                if(countAdjacentFixedCell(src) == 3){
                    extractSrcPiece(src, dst);
                    src = dst;
                }
                flag = trajectPiece(src, dst);
            }
            else {
                src = find((dst+Point(0, +1)).toInt());
                if( src == selected )return false;
                if(countAdjacentFixedCell(src) == 3){
                    extractSrcPiece(src, dst);                    
                    src = dst;
                }                
                flag = trajectPiece(src, dst);
            }
        } else {
            if(!Point(dst.y+1, dst.x).isIn(height(), width()) ||
                (Point(dst.y+1, dst.x).isIn(height(), width()) && fixed(dst.y+1, dst.x))){
                src = find((dst+Point(-1, 0)).toInt());
                if( src == selected )return false;
                if(countAdjacentFixedCell(src) == 3){
                    extractSrcPiece(src, dst);
                    src = dst;
                }
                flag = trajectPiece(src, dst);
            } else {
                src = find( (dst+Point(+1, 0)).toInt() );
                if( src == selected )return false;
                if(countAdjacentFixedCell(src) == 3){
                    extractSrcPiece(src, dst);       
                    src = dst;             
                }
                flag = trajectPiece(src, dst);
            }
        }
        if(flag){
            fixCandidate(dst);
            return true;
        }
    }
    else if(numRemain == 1){
        if(alignLastOne(dst)){
            fixCandidate(dst);
            return true;
        }
    }

    BOOST_ASSERT(false);
    return false;
}

template<typename Base>
bool AlignBoard<Base>::alignLastOne(Point dst)
{
    // dstの横のnot alignedなマスを探す.
    Point p(0, 0);
    rep(i, 4){
        const Direction dir(i);
        p = dst + Point::delta(dir);
        if( p.isIn(height(), width()) && fixed(p) && !isAligned(p)){
            break;
        };
    }
    // 本来入るべき番号srcを探す.
    Point src = find(p.toInt());

    if(src == dst){
        return swapAdjacentPiece(src, p);
    }

    // srcが入るtoの位置を探す.
    Point to;
    rep(i, 4){
        const Direction dir(i);
        to = p + Point::delta(dir);
        if( to.isIn(height(), width()) && !fixed(to) && to != dst){
            break;
        }
    }

    // srcが埋まっている場合はまわりのfixedを開放してとってくる.
    if(countAdjacentFixedCell(src) == 3){
        extractSrcPiece(src, to);
        src = to;
    }

    // ピースが正しく動かせる位置にある場合
    if(trajectPiece(src, to)){
        // ピースが正しく動かせても, selectedがはまってしまっている場合もある.
        fixed.set(to, true);
        moveToOptimally(dst); // selectedだけ動かす

        // 4方向を見て今いれようとしている, ブロックをさがして, その方向に進む
        rep(i, 4){
            const Direction dir(i);
            if( (dst+Point::delta(dir)).isIn(height(), width()) &&
                dst.toInt() == at(dst+Point::delta(dir))){
                move(dir);
                fixed.set(dst, true);
                dst += Point::delta(dir);
                break;
            }
        }
        rep(i, 4){
            const Direction dir(i);
            if( (dst+Point::delta(dir)).isIn(height(), width()) &&
                dst.toInt() == at(dst+Point::delta(dir))){
                move(dir);
                fixed.set(dst+Point::delta(dir), false);
                break;
            }
        }
        return true;
    }
    // ピースが動かせない場合
    else {
        // std::cout << src.toInt() << " " << at(dst) << std::endl;
        // std::cout << src << " " << to << std::endl;
        std::cout << "failed" << std::endl;
        std::cout << *this << std::endl;
        //exit(-1);
        return false;
    }
}

template<typename Base>
bool AlignBoard<Base>::swapAdjacentPiece(Point a, Point b)
{
    Direction dir(0);
    if(a.x == b.x){
        Point tmp = a + Point(0,-1);
        if(!tmp.isIn(height(), width()) || fixed(tmp) ){
            dir = Direction::Left;
        } else {
            dir = Direction::Right;
        }
        if( (int)(b.y < a.y) == (0 < (static_cast<int>(dir) & 2)) ) {
            std::swap(a, b);
        }
    } else if( a.y == b.y ){
        Point tmp = a + Point(-1,0);
        if(!tmp.isIn(height(), width()) || fixed(tmp)){
            dir = Direction::Up;
        } else {
            dir = Direction::Down;
        }
        if( (int)(b.x < a.x) == (0 < static_cast<int>(dir) )){
            std::swap(a, b);
        }
    }

    fixed.set(a, false);
    fixed.set(b, true);
    moveToOptimally(a);
    fixed.set(b, false);
    moveToOptimally(b);

    move( static_cast<Direction>((static_cast<int>(Direction::Down) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Right) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Down) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Left) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Up) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Up) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Right) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Down) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Left) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Down) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Right) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Up) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Up) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Left) + static_cast<int>(dir) ) & 0x03) );
    move( static_cast<Direction>((static_cast<int>(Direction::Down) + static_cast<int>(dir) ) & 0x03) );

    BOOST_ASSERT( isAligned(a) && isAligned(b) );
    fixed.set(a, true);
    fixed.set(b, true);
    return true;
}

template<typename Base>
std::ostream& operator<<(std::ostream& out, const AlignBoard<Base>& board)
{
    std::vector<Point> candidates;
    board.getCandidates(candidates);

    std::vector<std::vector<bool>> bb(board.height(), std::vector<bool>(board.width(), false));
    for(Point p : candidates){
        bb[p.y][p.x] = true;
    }

    rep(i, board.height()){
        rep(j, board.width()){
            out << '|';

            bool changed;
            if(bb[i][j]){
                out << util::ForeYellow;
                changed = true;
            }
            else if(!board.fixed(i, j) || !board.isAligned(i, j)){
                out << util::ForeRed;
                changed = true;
            }
            else{
                changed = false;
            }

            if(Point(i, j) == board.selected){
                out << util::BackGreen;
                changed = true;
            }

            out << boost::format("%02X") % int(board(i, j));

            if(changed){
                out << util::Default;
            }
        }
        out << "|\n";
    }

    return out;
}

} // end of namespace slide

#endif
