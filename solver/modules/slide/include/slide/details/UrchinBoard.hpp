#ifndef SLIDE_DETAILS_URCHIN_BOARD_HPP_
#define SLIDE_DETAILS_URCHIN_BOARD_HPP_

#include "../UrchinBoard.hpp"
#include "../Point.hpp"

#include <vector>

namespace slide
{

template<typename Base>
bool UrchinBoardBase<Base>::fixedCheck(int py, int px, int y, int x, int sH, int sW) const
{
	// isAlignedなマスに隣接する場合は候補にいれない.
	rep(i, 4){
		const Direction dir(i);
		Point tmp = Point(py, px) + Point::delta(dir);
		if( tmp.isIn(height(), width()) && fixed(tmp) && !isAligned(tmp))
			return true;
	}

	const uchar cnt = countAdjacentFixedCell(Point(py,px));
	if(cnt == 3)return false;
	if(cnt != 2){
		constexpr const int dx[] = {2,2,-2,-2};
		constexpr const int dy[] = {2,-2,2,-2};
		constexpr const int ddx[]= {1,1,-1,-1};
		constexpr const int ddy[]= {0,0,0,0};

		rep(i,4){
			Point a(py+dy[i],   px+dx[i]);
			Point b(py+dy[i]/2, px+dx[i]/2);
			Point c(py+ddy[i] , px+ddx[i]);
			if( !a.isIn(y,x,sH,sW) || fixed(c)) continue;
			if( fixed(a) && !fixed(b)) return true;
		}
		return false;
	}
	else { // cnt == 2
		Point checkCell(py,px);
		Point checkCell2(py,px);
		rep(i,4){
			const Direction dir = static_cast<Direction>(i);
			Point tmp = Point(py,px) + Point::delta(dir);
			if( !tmp.isIn(y,x,sH,sW) || fixed(tmp)){
				checkCell2 += Point::delta(dir.opposite());
				checkCell += Point::delta(dir.opposite());
				checkCell += Point::delta(dir.opposite());
			}
		}
		if( !checkCell.isIn(y,x,sH,sW) )return false;
		if( fixed(checkCell) && !fixed(checkCell2)) return true;
		return false;
	}
}

template<typename Base>
bool UrchinBoardBase<Base>::fixedCheck(int py, int px, int y, int x, int sH, int sW, int id) const
{
	// isAlignedなマスに隣接する場合は候補にいれない.
	rep(i, 4){
		const Direction dir(i);
		Point tmp = Point(py, px) + Point::delta(dir);
		if( tmp.isIn(height(), width()) && fixed(tmp) && !isAligned(tmp))
			return true;
	}

	int cnt = countAdjacentFixedCell(Point(py, px));
	if(cnt == 3)return false;

	// 端じゃないマスの時, px != x && px != x + sW - 1
	// countRowZero() == 2 ならば, そのマスを埋めると, 端のマスを埋めることが不可能となるので, その対策
	// pyも同じく
	if( py != y && py != y + sH - 1 && fixed.countRowZero(py) == 2)
		return true;
	if( px != x && px != x + sW - 1 && fixed.countColZero(px) == 2)
		return true;

	constexpr const int dx[] = {2, 2, -2, -2};
	constexpr const int dy[] = {2, -2, 2, -2};

	const Point tmp(py + dy[id], px + dx[id]);
	if( tmp.isIn(y, x, sH, sW) && !fixed(tmp))
		return false;
	return true;
}

template<typename Base>
void UrchinBoardBase<Base>::getCandidates(std::vector<Point>& cand) const
{
	cand.clear();
	int x;
	int y;
	int sH;
	int sW;
	fixed.largetstRect(y, x, sH, sW);

	// 4×4 以下
	if(sH <= 4 && sW <= 4){
		return;
	}

    // 残り1個だといれる.
	if(leftMost[y] == rightMost[y]){
		cand.emplace_back(y, leftMost[y]);
	}
	else if(fixed.countRowZero(y) == 2 && 
		!(fixed(y, x) && !isAligned(y, x)) && !(fixed(y, x + sW - 1) && !isAligned(y, x + sW - 1))
		){
		cand.emplace_back(y, leftMost[y]);
		cand.emplace_back(y, rightMost[y]);
	}
	else {
		// 空きますが2でない場合にいれる
		if( x != leftMost[y] && static_cast<int>(fixed.countColZero(leftMost[y])) == 2  );
		else if( !fixedCheck(y, leftMost[y], y, x, sH, sW, 0) ) cand.emplace_back(y, leftMost[y]);
		if( x+sW-1 != rightMost[y] && fixed.countColZero(rightMost[y]) == 2);
		else if( !fixedCheck(y, rightMost[y], y, x, sH, sW, 2)) cand.emplace_back(y,rightMost[y]);
	}

	// 残り１個だといれる.
	if( leftMost[y+sH-1] == rightMost[y+sH-1])
		cand.emplace_back(y+sH-1, leftMost[y+sH-1]);
	else if( fixed.countRowZero(y+sH-1) <= 2 &&
		!(fixed(y + sH - 1, x) && !isAligned(y + sH - 1, x)) && 
		!(fixed(y + sH - 1, x + sW - 1) && !isAligned(y + sH - 1, x + sW - 1)) 
		){
		cand.emplace_back(y+sH-1, leftMost[y+sH-1]);
		cand.emplace_back(y+sH-1, rightMost[y+sH-1]);
	}
	else {
		if( x != leftMost[y+sH-1] && fixed.countColZero(leftMost[y+sH-1]) == 2  );
		else if( !fixedCheck(y+sH-1, leftMost[y+sH-1], y, x, sH, sW, 1)) cand.emplace_back(y+sH-1, leftMost[y+sH-1]);
		if( x+sW-1 != rightMost[y+sH-1] && fixed.countColZero(rightMost[y+sH-1]) == 2);
		else if( !fixedCheck(y+sH-1, rightMost[y+sH-1], y, x, sH, sW, 3)) cand.emplace_back(y+sH-1, rightMost[y+sH-1]);
	}

	reps(i, y+1, y+sH-1){
		if(rightMost[i] - leftMost[i] <= 1)continue;
		if( !fixedCheck(i, leftMost[i], y, x, sH, sW) && (leftMost[i-1] > leftMost[i] || leftMost[i+1] > leftMost[i])){
			if(fixed.countColZero(leftMost[i]) <= 2)continue;
			cand.emplace_back(i, leftMost[i]);
		}
		if( !fixedCheck(i, rightMost[i], y, x, sH, sW) && (rightMost[i-1] < rightMost[i] || rightMost[i+1] < rightMost[i])){
			//if( rightMost[i] != x+sW-1 && fixed.countColZero(rightMost[i]) == 2)continue;
			if(fixed.countColZero(rightMost[i]) <= 2)continue;
			cand.emplace_back(i, rightMost[i]);
		}
	}

	// 列方向に見て空きますが1個だったらいれる.
	// ２個の場合は端っこがfixedでnot Alignedならばいれない.
	reps(i, y, y+sH){
		if( leftMost[i] == x ){
			const int cnt = fixed.countColZero(leftMost[i]);
			if(cnt == 1 || (cnt == 2 && !(fixed(y, leftMost[i]) && !isAligned(y, leftMost[i]))
				&& !(fixed(y + sH - 1, leftMost[i]) && !isAligned(y + sH - 1, leftMost[i]))
				))
				cand.emplace_back(i, leftMost[i]);
		}
		if( rightMost[i] == x+sW-1 ){
			const int cnt = fixed.countColZero(rightMost[i]);
			if(cnt == 1 || (cnt == 2 && !(fixed(y, rightMost[i]) && !isAligned(y, rightMost[i]))
				&& !(fixed(y + sH - 1, rightMost[i]) && !isAligned(y + sH - 1, rightMost[i]))
				))
				cand.emplace_back(i, rightMost[i]);
		}
	}

	// 同じものを消す
	std::sort(cand.begin(), cand.end(), [](const Point& a,const Point& b){
		return a.x != b.x ? a.x < b.x : a.y < b.y;
	});
	cand.erase(std::unique(cand.begin(), cand.end()), cand.end());

	// selected が入っていたら削除
	std::vector<Point>::iterator itr = std::find(cand.begin(), cand.end(), Point(at(selected)));
	if(itr != cand.end()){
		cand.erase(itr);
	}

	// selected の隣接マスが入っていたら削除
	rep(i, 4){
		const Direction dir(i);
		itr = std::find(cand.begin(), cand.end(), Point(at(selected)) + Point::delta(dir));
		if(itr != cand.end()){
			cand.erase(itr);
		}
	}
}

template<typename Base>
inline void UrchinBoardBase<Base>::fixCandidate(Point cand)
{
	fixed.set(cand);
	     if( leftMost[cand.y] == cand.x) ++leftMost[cand.y];
	else if(rightMost[cand.y] == cand.x) --rightMost[cand.y];
	else BOOST_ASSERT(false);
}

template<typename Base>
inline void UrchinBoardBase<Base>::unfixCandidate(Point cand)
{
	fixed.reset(cand);
	     if( leftMost[cand.y]-1 == cand.x) --leftMost[cand.y];
	else if(rightMost[cand.y]+1 == cand.x) ++rightMost[cand.y];
	else BOOST_ASSERT(false);
}

} // end of namespace slide

#endif
