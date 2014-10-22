#ifndef SLIDE_DETAILS_OCTOPUS_BOARD_HPP_
#define SLIDE_DETAILS_OCTOPUS_BOARD_HPP_

#include <vector>
#include <boost/assert.hpp>

#include "../OctopusBoard.hpp"
#include "../Point.hpp"

namespace slide
{

template<typename Base>
bool OctopusBoardBase<Base>::isCandidate(int py, int px, int y, int x, int sH, int sW, int id) const
{
	int cnt = countAdjacentFixedCell(Point(py, px));
	if(cnt == 3)return true;

	// 端じゃないマスの時, px != x && px != x + sW - 1
	// countRowZero() == 2 ならば, そのマスを埋めると, 端のマスを埋めることが不可能となるので, その対策
	// pyも同じく
	if( py != y && py != y + sH - 1 && fixed.countRowZero(py) == 2)
		return false;
	if( px != x && px != x + sW - 1 && fixed.countColZero(px) == 2)
		return false;

	constexpr const int dx[] = {2, 2, -2, -2};
	constexpr const int dy[] = {2, -2, 2, -2};

	const Point tmp(py + dy[id], px + dx[id]);
	if(!(tmp.isIn(y, x, sH, sW) && !fixed(tmp))) 
		return false;

	rep(i, 4){
		Direction dir(i);
		Point tmp = Point(py, px) + Point::delta(dir);
		if(tmp.isIn(height(), width()) && fixed(tmp) && !isAligned(tmp))
			return false;
	}
	return true;
}


template<typename Base>
void OctopusBoardBase<Base>::lengthCorner(std::vector<int>& vertical, std::vector<int>& horizontal, int y, int x, int sH, int sW) const
{
	for(int i=x;      i<x+sW-1 && fixed(y,i);      ++i) horizontal[0]++;
	for(int i=x+sW-1; i>x      && fixed(y,i);      --i) horizontal[2]++;
	for(int i=x;      i<x+sW-1 && fixed(y+sH-1,i); ++i) horizontal[1]++;
	for(int i=x+sW-1; i>x      && fixed(y+sH-1,i); --i) horizontal[3]++;

	for(int i=y;      i<y+sH-1 && fixed(i,x);      ++i) vertical[0]++;
	for(int i=y+sH-1; i>y      && fixed(i,x);      --i) vertical[1]++;
	for(int i=y;      i<y+sH-1 && fixed(i,x+sW-1); ++i) vertical[2]++;
	for(int i=y+sH-1; i>y      && fixed(i,x+sW-1); --i) vertical[3]++;

	/**************************************
	   	_, | : fixed zigsaw

        0               2
	    ______~~~~_______
	   |                 | 
 	   |                 |
 	   |                 |
 	   /                 /
 	   \                 /
 	   \                 /
	   |                 |
 	   |                 |
        -------~~~~-------
	     1               3   

	********************************************/
};

template<typename Base>
void OctopusBoardBase<Base>::getCandidates(std::vector<Point>& cand) const
{
	cand.clear();

	int x;
	int y;
	int sH;
	int sW;
	fixed.largetstRect(y, x, sH, sW);

	if(sH <= 4 && sW <= 4){
		return;
	}

	std::vector<int> vertical(4, 0);
	std::vector<int> horizontal(4, 0);
	// 角から何個が埋まっているかを取得
	lengthCorner(vertical, horizontal, y, x, sH, sW);


	// 空きますが2個だったら候補に入れる.
	if(sW - horizontal[2] - horizontal[0] <= 2 && 
		!(fixed(y, x) && !isAligned(y, x)) && !(fixed(y, x + sW - 1) && !isAligned(y, x + sW - 1)) ){
		cand.emplace_back(y, x + horizontal[0]);
		cand.emplace_back(y, x + sW - 1 - horizontal[2]);
	}
	if(sW - horizontal[3] - horizontal[1] <= 2 &&
		!(fixed(y + sH - 1, x) && !isAligned(y + sH - 1, x)) && !(fixed(y + sH - 1, x + sW - 1) && !isAligned(y + sH - 1, x + sW - 1)) ){
		cand.emplace_back(y + sH - 1, x + horizontal[1]);
		cand.emplace_back(y + sH - 1, x + sW - 1 - horizontal[3]);
	}
	if(sH - vertical[1] - vertical[0] <= 2 &&
		!(fixed(y, x) && !isAligned(y, x)) && !(fixed(y + sH - 1, x) && !isAligned(y + sH - 1, x)) ){
		cand.emplace_back(y + vertical[0], x);
		cand.emplace_back(y + sH - 1 - vertical[1], x);
	}
	if(sH - vertical[3] - vertical[2] <= 2 &&
		!(fixed(y, x + sW - 1) && !isAligned(y, x + sW - 1)) && !(fixed(y + sH - 1, x) && !isAligned(y + sH - 1, x + sW - 1)) ){
		cand.emplace_back(y + vertical[2], x + sW - 1);
		cand.emplace_back(y + sH - 1 - vertical[3], x + sW - 1);
	}

	// 空きますが1個だったら確実に入れる.
	if(sW - horizontal[2] - horizontal[0] == 1){
		cand.emplace_back(y, x + horizontal[0]);
	}
	if(sW - horizontal[3] - horizontal[1] == 1){
		cand.emplace_back(y + sH - 1, x + horizontal[1]);
	}
	if(sH - vertical[1] - vertical[0] == 1){
		cand.emplace_back(y + vertical[0], x);
	}
	if(sH - vertical[3] - vertical[2] == 1){
		cand.emplace_back(y + vertical[2], x + sW - 1);
	}



	rep(i, 4){
		//int cx = x + (sW - 1) * (((i >> 1) & 1));
		int cx = x + std::abs(horizontal[i] - (sW - 1) * (((i >> 1) & 1)));
		int cy = y + (sH-1) * (i & 1);
	    if(horizontal[i] != 0) {
	    	if(!isCandidate(cy, cx, y, x, sH, sW, i)){
	    		continue;
	    	}
	    	cand.emplace_back(cy, cx);
	    } 
	    else {
	    	if(!isCandidate(cy, cx, y, x, sH, sW, i)) continue;
	    	cand.emplace_back( cy, cx );
	    }
	}

	rep(i, 4){
		int cx = x + (sW - 1) * (((i >> 1) & 1));
		int cy = y + std::abs(vertical[i] - (sH - 1) * (i & 1));
		if(!isCandidate(cy, cx, y, x, sH, sW, i)) continue;
		cand.emplace_back( cy, cx);
	}


	// 同じものを消す
	std::sort(cand.begin(), cand.end(), [](const Point& a,const Point& b){
		return (a.x != b.x ? a.x < b.x : a.y < b.y);
	});
	cand.erase(std::unique(cand.begin(), cand.end()), cand.end());

	// selected が入っていたら削除
	std::vector<Point>::iterator itr = std::find(cand.begin(), cand.end(), Point(at(selected)));
	if(itr != cand.end()){
		cand.erase(itr);
	}

	// selectedの隣接マスも候補から削除
	rep(i, 4){
		const Direction dir(i);
		itr = std::find(cand.begin(), cand.end(), Point(at(selected)) + Point::delta(dir));
		if(itr != cand.end()){
			cand.erase(itr);
		}
	}
}

    
} // end of namespace slide

#endif
