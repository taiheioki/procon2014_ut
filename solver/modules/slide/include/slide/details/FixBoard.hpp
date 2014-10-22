#ifndef SLIDE_DETAILS_FIX_BOARD_HPP_
#define SLIDE_DETAILS_FIX_BOARD_HPP_

#include <algorithm>
#include <cstdlib>
#include <queue>
#include <stack>
#include <utility>

#include "../FixBoard.hpp"
#include "../ManhattanFeature.hpp"
#include "util/color.hpp"
#include "util/StopWatch.hpp"

namespace slide
{

template<typename Base>
inline bool FixBoard<Base>::searchMove(Point src, Point dst, char pre[arrayH][arrayW]) const
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(src));

    rep(i, height()) rep(j, width()){
        pre[i][j] = -1; // not reached yet
    }

    // heuristic function
    /*
    auto heuristic = [](Point a, Point b){
        return (a-b).l1norm();
    };
    */

    bool found = false;
    std::queue<Point> Q;
    Q.push(src);
    pre[src.y][src.x] = -2; // the start point

    // BFS
    while(!Q.empty()){
        const Point p = Q.front();
        Q.pop();

        if(p == dst){
            found = true;
            break;
        }

        rep(k, 4){
            const Direction dir(k);
            const Point n = p + Point::delta(dir);
            if(!n.isIn(height(), width()) || fixed(n) || pre[n.y][n.x] != -1){
                continue;
            }

            pre[n.y][n.x] = k;
            Q.push(n);
        }
    }

    return found;
}

template<typename Base>
inline bool FixBoard<Base>::searchMoveOptimally(Point src, Point dst, char pre[arrayH][arrayW]) const
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(src));

    char manhattan[arrayH][arrayW];
    rep(i, height()) rep(j, width()){
        manhattan[i][j] = 127; // not reached yet
    }

    // heuristic function
    /*
    auto heuristic = [](Point a, Point b){
        return (a-b).l1norm();
    };
    */

    bool found = false;
    std::queue<Point> Q;
    Q.push(src);
    manhattan[src.y][src.x] = 0;

    // BFS
    while(!Q.empty()){
        const Point p = Q.front();
        Q.pop();

        if(p == dst){
            found = true;
            break;
        }

        rep(k, 4){
            const Direction dir = Direction(k);
            const Point n = p + Point::delta(dir);
            if(!n.isIn(height(), width()) || fixed(n) || manhattan[n.y][n.x] == -128){
                continue;
            }

            const char nextManhattan = manhattan[p.y][p.x] + ManhattanFeature::delta(*this, p, dir);

            if(manhattan[n.y][n.x] > nextManhattan){
                if(manhattan[n.y][n.x] == 127){
                    Q.push(n);
                }
                pre[n.y][n.x] = k;
                manhattan[n.y][n.x] = nextManhattan;
            }
        }

        manhattan[p.y][p.x] = -128;
    }

    return found;
}

template<typename Base>
inline bool FixBoard<Base>::moveTo(Point dst)
{
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(isSelected());

    // マンハッタン距離が2以下の場合について枝刈り
    const int dist = (selected - dst).l1norm();
    if(dist == 0){
        return true;
    }
    else if(dist == 1){
        const Point d = dst - selected;
        move(d.y != 0 ? (d.y < 0 ? Direction::Up : Direction::Down) : (d.x < 0 ? Direction::Left : Direction::Right));
        return true;
    }
    if(dist == 2){
        if(selected.y == dst.y){ // 横一直線
            const int y = selected.y;
            const Point mid(y, (selected.x + dst.x) >> 1);
            if(!fixed(mid)){
                moveHorizontally(dst.x);
                return true;
            }
            if(y>0   && !fixed(y-1, selected.x) && !fixed(y-1, mid.x) && !fixed(y-1, dst.x)){
                moveUp();
                moveHorizontally(dst.x);
                moveDown();
                return true;
            }
            if(y<Base::H-1 && !fixed(y+1, selected.x) && !fixed(y+1, mid.x) && !fixed(y+1, dst.x)){
                moveDown();
                moveHorizontally(dst.x);
                moveUp();
                return true;
            }
        }
        else if(selected.x == dst.x){ // 縦一直線
            const int x = selected.x;
            const Point mid((selected.y + dst.y) >> 1, x);
            if(!fixed(mid)){
                moveVertically(dst.y);
                return true;
            }
            if(x>0   && !fixed(selected.y, x-1) && !fixed(mid.y, x-1) && !fixed(dst.y, x-1)){
                moveLeft();
                moveVertically(dst.y);
                moveRight();
                return true;
            }
            if(x<Base::W-1 && !fixed(selected.y, x+1) && !fixed(mid.y, x+1) && !fixed(dst.y, x+1)){
                moveRight();
                moveVertically(dst.y);
                moveLeft();
                return true;
            }
        }
        else{ // 対角線上    
            if(!fixed(selected.y, dst.x)){
                moveHorizontally(dst.x);
                moveVertically(dst.y);
                return true;
            }
            if(!fixed(dst.y, selected.x)){
                moveVertically(dst.y);
                moveHorizontally(dst.x);
                return true;
            }
        }
    }

    // 探索
    char pre[arrayH][arrayW];
    if(!searchMove(selected, dst, pre)){
        return false;
    }

    // 実際に移動
    std::stack<Direction> dirs;
    while(dst != selected){
        const Direction dir = Direction(pre[dst.y][dst.x]);
        dst -= Point::delta(dir);
        dirs.push(dir);
    }

    while(!dirs.empty()){
        const Direction dir = dirs.top();
        dirs.pop();
        move(dir);
    }

    return true;
}

template<typename Base>
inline bool FixBoard<Base>::moveToTwoNeighborOptimally(Point dst)
{
    if(selected.y == dst.y){
        // 横一直線
        const int y = selected.y;

        if(selected.x < dst.x){
            // 右に二つ

            if(!fixed(y, selected.x + 1)){
                // →→
                moveRight();
                moveRight();
                return true;
            }

            bool up_ok   = y>0          && !fixed(y-1, selected.x) && !fixed(y-1, selected.x+1) && !fixed(y-1, dst.x);
            bool down_ok = y<height()-1 && !fixed(y+1, selected.x) && !fixed(y+1, selected.x+1) && !fixed(y+1, dst.x);

            // どちらがマンハッタン距離の減少量が大きいかをチェック
            if(up_ok && down_ok){
                int up_dm;
                {
                    Point sel = selected;
                    up_dm = ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    up_dm += ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    up_dm += ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    up_dm += ManhattanFeature::deltaDown(*this, sel);
                }

                int down_dm;
                {
                    Point sel = selected;
                    down_dm = ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    down_dm += ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    down_dm += ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    down_dm += ManhattanFeature::deltaUp(*this, sel);
                }

                if(up_dm < down_dm) down_ok = false;
                else up_ok = false;
            }

            if(up_ok){
                // →→
                // ↑↓
                moveUp();
                moveRight();
                moveRight();
                moveDown();
                return true;
            }
            if(down_ok){
                // ↓↑
                // →→
                moveDown();
                moveRight();
                moveRight();
                moveUp();
                return true;
            }
        }
        else{
            // 左に二つ

            if(!fixed(y, selected.x - 1)){
                // ←←
                moveLeft();
                moveLeft();
                return true;
            }

            bool up_ok   = y>0          && !fixed(y-1, selected.x) && !fixed(y-1, selected.x-1) && !fixed(y-1, dst.x);
            bool down_ok = y<height()-1 && !fixed(y+1, selected.x) && !fixed(y+1, selected.x-1) && !fixed(y+1, dst.x);

            // どちらがマンハッタン距離の減少量が大きいかをチェック
            if(up_ok && down_ok){
                int up_dm;
                {
                    Point sel = selected;
                    up_dm = ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    up_dm += ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    up_dm += ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    up_dm += ManhattanFeature::deltaDown(*this, sel);
                }

                int down_dm;
                {
                    Point sel = selected;
                    down_dm = ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    down_dm += ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    down_dm += ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    down_dm += ManhattanFeature::deltaUp(*this, sel);
                }

                if(up_dm < down_dm) down_ok = false;
                else up_ok = false;
            }

            if(up_ok){
                // ←←
                // ↓↑
                moveUp();
                moveLeft();
                moveLeft();
                moveDown();
                return true;
            }
            if(down_ok){
                // ↑↓
                // ←←
                moveDown();
                moveLeft();
                moveLeft();
                moveUp();
                return true;
            }
        }
    }
    else if(selected.x == dst.x){ // 縦一直線
        // 縦一直線
        const int x = selected.x;

        if(selected.y < dst.y){
            // 下に二つ

            if(!fixed(selected.y + 1, x)){
                // ↓
                // ↓
                moveDown();
                moveDown();
                return true;
            }

            bool left_ok  = x>0         && !fixed(selected.y, x-1) && !fixed(selected.y+1, x-1) && !fixed(dst.y, x-1);
            bool right_ok = x<width()-1 && !fixed(selected.y, x+1) && !fixed(selected.y+1, x+1) && !fixed(dst.y, x+1);

            // どちらがマンハッタン距離の減少量が大きいかをチェック
            if(left_ok && right_ok){
                int left_dm;
                {
                    Point sel = selected;
                    left_dm = ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    left_dm += ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    left_dm += ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    left_dm += ManhattanFeature::deltaRight(*this, sel);
                }

                int right_dm;
                {
                    Point sel = selected;
                    right_dm = ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    right_dm += ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    right_dm += ManhattanFeature::deltaDown(*this, sel);
                    sel += Point::delta(Direction::Down);
                    right_dm += ManhattanFeature::deltaLeft(*this, sel);
                }

                if(left_dm < right_dm) right_ok = false;
                else left_ok = false;
            }

            if(left_ok){
                // ↓←
                // ↓→
                moveLeft();
                moveDown();
                moveDown();
                moveRight();
                return true;
            }
            if(right_ok){
                // →↓
                // ←↓
                moveRight();
                moveDown();
                moveDown();
                moveLeft();
                return true;
            }
        }
        else{
            // 上に二つ

            if(!fixed(selected.y - 1, x)){
                // ↑
                // ↑
                moveUp();
                moveUp();
                return true;
            }

            bool left_ok  = x>0         && !fixed(selected.y, x-1) && !fixed(selected.y-1, x-1) && !fixed(dst.y, x-1);
            bool right_ok = x<width()-1 && !fixed(selected.y, x+1) && !fixed(selected.y-1, x+1) && !fixed(dst.y, x+1);

            // どちらがマンハッタン距離の減少量が大きいかをチェック
            if(left_ok && right_ok){
                int left_dm;
                {
                    Point sel = selected;
                    left_dm = ManhattanFeature::deltaLeft(*this, sel);
                    sel += Point::delta(Direction::Left);
                    left_dm += ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    left_dm += ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    left_dm += ManhattanFeature::deltaRight(*this, sel);
                }

                int right_dm;
                {
                    Point sel = selected;
                    right_dm = ManhattanFeature::deltaRight(*this, sel);
                    sel += Point::delta(Direction::Right);
                    left_dm += ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    left_dm += ManhattanFeature::deltaUp(*this, sel);
                    sel += Point::delta(Direction::Up);
                    right_dm += ManhattanFeature::deltaLeft(*this, sel);
                }

                if(left_dm < right_dm) right_ok = false;
                else left_ok = false;
            }

            if(left_ok){
                // ↑→
                // ↑←
                moveLeft();
                moveUp();
                moveUp();
                moveRight();
                return true;
            }
            if(right_ok){
                // ←↑
                // →↑
                moveRight();
                moveUp();
                moveUp();
                moveLeft();
                return true;
            }
        }
    }
    else{ // 対角線上
        if(!fixed(selected.y, dst.x)){
            moveHorizontally(dst.x);
            moveVertically(dst.y);
            return true;
        }
        if(!fixed(dst.y, selected.x)){
            moveVertically(dst.y);
            moveHorizontally(dst.x);
            return true;
        }
    }

    return moveToOptimallyBySearch(dst);
}

template<typename Base>
inline bool FixBoard<Base>::moveToOptimallyBySearch(Point dst)
{
    // 探索
    char pre[arrayH][arrayW];
    if(!searchMoveOptimally(selected, dst, pre)){
        return false;
    }

    // 実際に移動
    std::stack<Direction> dirs;
    while(dst != selected){
        const Direction dir = Direction(pre[dst.y][dst.x]);
        dst -= Point::delta(dir);
        dirs.push(dir);
    }

    while(!dirs.empty()){
        const Direction dir = dirs.top();
        dirs.pop();
        move(dir);
    }

    return true;
}

template<typename Base>
inline bool FixBoard<Base>::moveToOptimally(Point dst)
{
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(isSelected());

    // マンハッタン距離が2以下の場合について枝刈り
    const int dist = (selected - dst).l1norm();
    if(dist == 0){
        // selected = dst
        return true;
    }
    else if(dist == 1){
        // 一近傍
        const Point d = dst - selected;
        move(d.y != 0 ? (d.y < 0 ? Direction::Up : Direction::Down) : (d.x < 0 ? Direction::Left : Direction::Right));
        return true;
    }
    if(dist == 2){
        // 二近傍
        return moveToTwoNeighborOptimally(dst);
    }

    return moveToOptimallyBySearch(dst);
}

template<typename Base>
template<uchar Rot_>
inline bool FixBoard<Base>::moveToLinearTwoNeighborMoreOptimally()
{
    constexpr Direction UpDir(Rot_);
    constexpr Direction RightDir = UpDir.cw();
    constexpr Direction DownDir  = UpDir.opposite();
    constexpr Direction LeftDir  = UpDir.ccw();
    constexpr Point Up           = Point::delta(UpDir);
    constexpr Point Right        = Point::delta(RightDir);
    constexpr Point Down         = Point::delta(DownDir);
    constexpr Point Left         = Point::delta(LeftDir);

    bool ok[10] = {false};

    // 上方向
    if((selected + Up).isIn(height(), width()) &&
        !fixed(selected + Up) && !fixed(selected + Up + Right) && !fixed(selected + Up + Right*2)){
        ok[0] = true;

        if((selected + Up + Left).isIn(height(), width()) && !fixed(selected + Left) && !fixed(selected + Up + Left)){
            ok[1] = true;
        }

        if((selected + Up + Right*3).isIn(height(), width()) && !fixed(selected + Right*3) && !fixed(selected + Up + Right*3)){
            ok[2] = true;
        }

        if((selected + Up*2).isIn(height(), width()) && !fixed(selected + Up*2 + Right)){
            if(!fixed(selected + Up*2)){
                ok[3] = true;
            }
            if(!fixed(selected + Up*2 + Right*2)){
                ok[4] = true;
            }
        }
    }

    // 下方向
    if((selected + Down).isIn(height(), width()) &&
        !fixed(selected + Down) && !fixed(selected + Down + Right) && !fixed(selected + Down + Right*2)){
        ok[5] = true;

        if((selected + Down + Left).isIn(height(), width()) && !fixed(selected + Left) && !fixed(selected + Down + Left)){
            ok[6] = true;
        }

        if((selected + Down + Right*3).isIn(height(), width()) && !fixed(selected + Right*3) && !fixed(selected + Down + Right*3)){
            ok[7] = true;
        }

        if((selected + Down*2).isIn(height(), width()) && !fixed(selected + Down*2 + Right)){
            if(!fixed(selected + Down*2)){
                ok[8] = true;
            }
            if(!fixed(selected + Down*2 + Right*2)){
                ok[9] = true;
            }
        }
    }

    int eval[10];

    if(ok[0]){
        Point sel;
        int baseDelta[4];
        int baseSum;

        // →→↓
        // ↑  □
        sel = selected;
        baseDelta[0] = ManhattanFeature::delta(*this, sel, UpDir);    sel += Up;
        baseDelta[1] = ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
        baseDelta[2] = ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
        baseDelta[3] = ManhattanFeature::delta(*this, sel, DownDir);

        baseSum = baseDelta[0] + baseDelta[1] + baseDelta[2] + baseDelta[3];
        eval[0] = 2 + baseSum;

        if(ok[1]){
            // →→→↓
            // ↑←  □
            sel = selected;
            eval[1] = 6 + baseSum - baseDelta[0];
            eval[1] += ManhattanFeature::delta(*this, sel, LeftDir);  sel += Left;
            eval[1] += ManhattanFeature::delta(*this, sel, UpDir);    sel += Up;
            eval[1] += ManhattanFeature::delta(*this, sel, RightDir);
        }

        if(ok[2]){
            // →→→↓
            // ↑  □←
            eval[2] = 6 + baseSum - baseDelta[3];
            sel = selected + Up + Right*2;
            eval[2] += ManhattanFeature::delta(*this, sel, RightDir);  sel += Right;
            eval[2] += ManhattanFeature::delta(*this, sel, DownDir);   sel += Down;
            eval[2] += ManhattanFeature::delta(*this, sel, LeftDir);
        }

        if(ok[3]){
            // →↓
            // ↑→↓
            // ↑  □
            eval[3] = 6 + baseSum - baseDelta[1];
            sel = selected + Up;
            eval[3] += ManhattanFeature::delta(*this, sel, UpDir);    sel += Up;
            eval[3] += ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
            eval[3] += ManhattanFeature::delta(*this, sel, DownDir);
        }

        if(ok[4]){
            //   →↓
            // →↑↓
            // ↑  □
            eval[4] = 6 + baseSum - baseDelta[2];
            sel = selected + Up + Right;
            eval[4] += ManhattanFeature::delta(*this, sel, UpDir);    sel += Up;
            eval[4] += ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
            eval[4] += ManhattanFeature::delta(*this, sel, DownDir);
        }
    }

    if(ok[5]){
        Point sel;
        int baseDelta[4];
        int baseSum;

        // ↓  □
        // →→↑
        sel = selected;
        baseDelta[0] = ManhattanFeature::delta(*this, sel, DownDir);  sel += Down;
        baseDelta[1] = ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
        baseDelta[2] = ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
        baseDelta[3] = ManhattanFeature::delta(*this, sel, UpDir);

        baseSum = baseDelta[0] + baseDelta[1] + baseDelta[2] + baseDelta[3];
        eval[5] = 2 + baseSum;

        if(ok[6]){
            // ↓←  □
            // →→→↑
            sel = selected;
            eval[6] = 6 + baseSum - baseDelta[0];
            eval[6] += ManhattanFeature::delta(*this, sel, LeftDir);  sel += Left;
            eval[6] += ManhattanFeature::delta(*this, sel, DownDir);  sel += Down;
            eval[6] += ManhattanFeature::delta(*this, sel, RightDir);
        }

        if(ok[7]){
            // ↓  □←
            // →→→↑
            eval[7] = 6 + baseSum - baseDelta[3];
            sel = selected + Down + Right*2;
            eval[7] += ManhattanFeature::delta(*this, sel, RightDir);  sel += Right;
            eval[7] += ManhattanFeature::delta(*this, sel, UpDir);     sel += Up;
            eval[7] += ManhattanFeature::delta(*this, sel, LeftDir);
        }

        if(ok[8]){
            // ↓  □
            // ↓→↑
            // →↑
            eval[8] = 6 + baseSum - baseDelta[1];
            sel = selected + Down;
            eval[8] += ManhattanFeature::delta(*this, sel, DownDir);  sel += Down;
            eval[8] += ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
            eval[8] += ManhattanFeature::delta(*this, sel, UpDir);
        }

        if(ok[9]){
            // ↓  □
            // →↓↑
            //   →↑
            eval[9] = 6 + baseSum - baseDelta[2];
            sel = selected + Down + Right;
            eval[9] += ManhattanFeature::delta(*this, sel, DownDir);  sel += Down;
            eval[9] += ManhattanFeature::delta(*this, sel, RightDir); sel += Right;
            eval[9] += ManhattanFeature::delta(*this, sel, UpDir);
        }
    }

    int method = -1;
    int best   = 1 << 29;
    rep(i, 10){
        const int id = (i%2)*5 + i/2;
        if(ok[id] && eval[id] < best){
            method = id;
            best = eval[id];
        }
    }

    if(method == -1){
        return false;
    }

    if(method < 5){
        if(method < 3){
            if(method == 0){
                move(UpDir);
                move(RightDir);
                move(RightDir);
                move(DownDir);
            }
            else if(method == 1){
                move(LeftDir);
                move(UpDir);
                move(RightDir);
                move(RightDir);
                move(RightDir);
                move(DownDir);
            }
            else{
                move(UpDir);
                move(RightDir);
                move(RightDir);
                move(RightDir);
                move(DownDir);
                move(LeftDir);
            }
        }
        else{
            if(method == 3){
                move(UpDir);
                move(UpDir);
                move(RightDir);
                move(DownDir);
                move(RightDir);
                move(DownDir);
            }
            else{
                move(UpDir);
                move(RightDir);
                move(UpDir);
                move(RightDir);
                move(DownDir);
                move(DownDir);
            }
        }
    }
    else{
        if(method < 8){
            if(method == 5){
                move(DownDir);
                move(RightDir);
                move(RightDir);
                move(UpDir);
            }
            else if(method == 6){
                move(LeftDir);
                move(DownDir);
                move(RightDir);
                move(RightDir);
                move(RightDir);
                move(UpDir);
            }
            else{
                move(DownDir);
                move(RightDir);
                move(RightDir);
                move(RightDir);
                move(UpDir);
                move(LeftDir);
            }
        }
        else{
            if(method == 8){
                move(DownDir);
                move(DownDir);
                move(RightDir);
                move(UpDir);
                move(RightDir);
                move(UpDir);
            }
            else{
                move(DownDir);
                move(RightDir);
                move(DownDir);
                move(RightDir);
                move(UpDir);
                move(UpDir);
            }
        }
    }

    return true;
}

template<typename Base>
inline bool FixBoard<Base>::moveToTwoNeighborMoreOptimally(Point dst)
{
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(isSelected());   
    BOOST_ASSERT((selected - dst).l1norm() == 2);

    if(selected.y == dst.y){
        // 横一直線
        if(selected.x < dst.x){
            // →→
            if(moveToLinearTwoNeighborMoreOptimally<Direction::Up>()){
                return true;
            }
        }
        else{
            // ←←
            if(moveToLinearTwoNeighborMoreOptimally<Direction::Down>()){
                return true;
            }
        }
    }
    else if(selected.x == dst.x){
        // 縦一直線
        if(selected.y < dst.y){
            // ↓
            // ↓
            if(moveToLinearTwoNeighborMoreOptimally<Direction::Right>()){
                return true;
            }
        }
        else{
            // ↑
            // ↑
            if(moveToLinearTwoNeighborMoreOptimally<Direction::Left>()){
                return true;
            }
        }
    }

    return moveToTwoNeighborOptimally(dst);
}

template<typename Base>
inline int FixBoard<Base>::countMoveTo(Point src, Point dst) const
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(src));

    // マンハッタン距離が2以下の場合について枝刈り
    const int dist = (src - dst).l1norm();
    if(dist <= 1){
        return dist;
    }
    if(dist == 2){
        return countMoveToTwoNeighbor(src, dst);
    }

    return countMoveToBySearch(src, dst);
}

template<typename Base>
inline int FixBoard<Base>::countMoveToTwoNeighbor(Point src, Point dst) const
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(src));
    BOOST_ASSERT((src - dst).l1norm() == 2);

    if(src.y == dst.y){ // 横一直線
        const int y = src.y;
        const Point mid(y, (src.x + dst.x) >> 1);
        if(!fixed(mid)) return 2;
        if(y>0          && !fixed(y-1, src.x) && !fixed(y-1, mid.x) && !fixed(y-1, dst.x)) return 5;
        if(y<height()-1 && !fixed(y+1, src.x) && !fixed(y+1, mid.x) && !fixed(y+1, dst.x)) return 5;
    }
    else if(src.x == dst.x){ // 縦一直線
        const int x = src.x;
        const Point mid((src.y + dst.y) >> 1, x);
        if(!fixed(mid)) return 2;
        if(x>0         && !fixed(src.y, x-1) && !fixed(mid.y, x-1) && !fixed(dst.y, x-1)) return 5;
        if(x<width()-1 && !fixed(src.y, x+1) && !fixed(mid.y, x+1) && !fixed(dst.y, x+1)) return 5;
    }
    else{ // 対角線上    
        if(!fixed(src.y, dst.x) || !fixed(dst.y, src.x)){
            return 2;
        }
    }

    return countMoveToBySearch(src, dst);
}

template<typename Base>
inline int FixBoard<Base>::countMoveTo(Point dst) const
{
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(isSelected());
    return countMoveTo(selected, dst);
}

template<typename Base>
inline int FixBoard<Base>::countMoveToBySearch(Point src, Point dst) const
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(src));

    // 探索
    char pre[arrayH][arrayW];
    if(!searchMove(src, dst, pre)){
        return -1;
    }

    // 数える
    int ret = 0;
    for(; dst != src; ++ret){
        const Direction dir = Direction(pre[dst.y][dst.x]);
        dst -= Point::delta(dir);
    }

    return ret;
}

/*
template<typename Base>
void FixBoard<Base>::countMoveTo(Point src, const std::vector<Point>& dst, std::vector<int>& ret) const
{
    BOOST_ASSERT(std::all_of(dst.begin(), dst.end(), [&](Point d){ return d.isIn(height(), width()); }));
}

template<typename Base>
void FixBoard<Base>::countMoveTo(const std::vector<Point>& dst, std::vector<int>& ret) const
{
    BOOST_ASSERT(isSelected());
    return countMoveTo(selected, dst, ret);
}
*/

template<typename Base>
bool FixBoard<Base>::trajectPiece(Point src, Point dst)
{
    BOOST_ASSERT(src.isIn(height(), width()));
    BOOST_ASSERT(dst.isIn(height(), width()));
    BOOST_ASSERT(isSelected());
    BOOST_ASSERT(selected != src);
    BOOST_ASSERT(!fixed(src));
    BOOST_ASSERT(!fixed(dst));
    BOOST_ASSERT(!fixed(selected));

    if(src == dst){
        return true;
    }

    struct Data
    {
        Point pos;
        Direction lastMove, lastMove2;
        int cost, estimate;

        Data() = default;
        constexpr Data(Point pos, Direction lastMove, Direction lastMove2, int cost, int estimate)
            : pos(pos), lastMove(lastMove), lastMove2(lastMove2), cost(cost), estimate(estimate) {}

        bool operator< (const Data& data)const {
            return
            estimate == data.estimate ?
                cost == data.cost ?
                    lastMove == data.lastMove ?
                        lastMove2 == data.lastMove2 ?
                            pos.y == data.pos.y ?
                                pos.x < data.pos.x :
                            pos.y < data.pos.y :
                        lastMove2 < data.lastMove2 :
                    lastMove < data.lastMove :
                cost > data.cost :
            estimate > data.estimate;
        }
    };

    // initialize log
    char pre[arrayH][arrayW][4];
    int cost[arrayH][arrayW][4];
    rep(i, arrayH) rep(j, arrayW) rep(k, 4){
        cost[i][j][k] = -1; // not reached yet
    }

    // heuristic function
    auto heuristic = [](Point a, Point b){
        int dy = std::abs(a.y - b.y);
        int dx = std::abs(a.x - b.x);
        return (std::min(dy, dx)*6 + std::abs(dy - dx)*5) * 2;
    };

    std::priority_queue<Data> Q;

    // move to adjacent of src
    rep(k, 4){
        const Direction dir(k);
        const Point nextPos = src + Point::delta(dir);
        if(!nextPos.isIn(height(), width()) || fixed(nextPos)){
            continue;
        }

        fixed.set(src);
        const int minMove = countMoveTo(nextPos);
        fixed.reset(src);
        if(minMove == -1){
            continue;
        }

        Q.emplace(nextPos, dir, dir, minMove + 1, minMove + 1 + heuristic(nextPos, dst));
    }

    bool found = false;
    Data last;

    // A* search
    while(!Q.empty()){
        const Data data = Q.top();
        Q.pop();

        int& c = cost[data.pos.y][data.pos.x][int(data.lastMove)];
        if(c != -1){
            continue;
        }
        pre[data.pos.y][data.pos.x][int(data.lastMove)] = char(data.lastMove2);
        c = data.cost;

        if(data.pos == dst){
            found = true;
            last = data;
            break;
        }

        rep(k, 4){
            const Direction dir(k);
            if(dir == data.lastMove.opposite()){
                continue;
            }

            const Point nextPos = data.pos + Point::delta(dir);
            if(!nextPos.isIn(height(), width()) || fixed(nextPos)){
                continue;
            }

            fixed.set(data.pos);
            const int minMove = countMoveToTwoNeighbor(data.pos - Point::delta(data.lastMove), nextPos);
            fixed.reset(data.pos);
            if(minMove == -1){
                continue;
            }

            const int nextCost = data.cost + minMove + 1;
            Q.emplace(nextPos, dir, data.lastMove, nextCost, nextCost + heuristic(nextPos, dst));
        }
    }

    if(!found){
        return false;
    }

    // 実際に移動
    std::stack<Direction> dirs;

    do{
        dirs.push(last.lastMove);
        const Direction dir = Direction(pre[last.pos.y][last.pos.x][int(last.lastMove)]);
        last.pos -= Point::delta(last.lastMove);
        last.lastMove = dir;
    }while(last.pos != src);

    while(!dirs.empty()){
        const Direction dir = dirs.top();
        dirs.pop();

        const Point next = src + Point::delta(dir);
        fixed.set(src);
        if((selected - next).l1norm() == 2){
            //moveToTwoNeighborMoreOptimally(next);
            moveToTwoNeighborOptimally(next);
            //moveTo(next);
        }
        else{
            moveToOptimally(next);
        }
        fixed.reset(src);

        move(dir.opposite());
        src = next;
    }

    return true;
}

template<typename Base>
inline uchar FixBoard<Base>::countAdjacentFixedCell(Point p) const 
{
    uchar cnt = 0;
    rep(i, 4){
        const Direction dir(i);
        const Point tmp = p + Point::delta(dir);
        if(!tmp.isIn(height(), width()) || fixed(tmp)){
            cnt++;
        }
    }
    return cnt;
}

template<typename Base>
std::ostream& operator<<(std::ostream& out, const FixBoard<Base>& board)
{
    rep(i, board.height()){
        rep(j, board.width()){
            out << "|";

            if(board.fixed(i, j)){
                out << util::ForeGreen;
            }
            if(board.selected == Point(i, j)){
                out << util::ForeRed;
            }

            out << boost::format("%02X") % int(board(i, j));

            if(board.selected == Point(i, j) || board.fixed(i, j)){
                out << util::ForeDefault;
            }
        }
        out << "|\n";
    }

    return out;
}

} // end of namespace slide

#endif
