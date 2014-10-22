#ifndef SLIDE_POINT_HPP_
#define SLIDE_POINT_HPP_

#include <array>
#include <cstdlib>
#include <iostream>

#include <boost/assert.hpp>

#include "Direction.hpp"
#include "util/abs.hpp"
#include "util/define.hpp"

#ifdef __gcc__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace slide
{

class Point
{
public:
	int y;
	int x;

	Point() = default;

	constexpr Point(int y, int x) : y(y), x(x) {}

	constexpr explicit Point(uchar v, int w = MAX_DIVISION_NUM) : Point(v/w, v%w) {}

	constexpr uchar toInt(int w = MAX_DIVISION_NUM) const {
		return y * w + x;
	}

	constexpr int l1norm() const {
		return util::abs(y) + util::abs(x);
	}

	constexpr bool isIn(int height, int width) const {
		return isIn(0, 0, height, width);
	}

	constexpr bool isIn(int py, int px, int height, int width) const {
		return py <= y && y < py+height && px <= x && x < px+width;
	}

	constexpr Point operator+ (const Point& p) const {
		return Point(y+p.y, x+p.x);
	}

	constexpr Point operator- (const Point& p) const {
		return Point(y-p.y, x-p.x);
	}

	constexpr Point operator+ () const {
		return *this;
	}

	constexpr Point operator- () const {
		return Point(-y, -x);
	}

	Point& operator+=(const Point& p){
		y += p.y;
		x += p.x;
		return *this;
	}

	Point& operator-=(const Point& p){
		y -= p.y;
		x -= p.x;
		return *this;
	}

	constexpr Point operator* (const int v) const {
		return Point(y*v, x*v);
	}

	Point& operator*=(const int v) {
		y *= v;
		x *= v;
		return *this;
	}

	constexpr bool operator==(const Point& p) const {
		return y == p.y && x == p.x;
	}

	constexpr bool operator!=(const Point& p) const {
		return y != p.y || x != p.x;
	}

	bool operator< (const Point& p) const {
		return (x != p.x ? x < p.x : y < p.y);
	}

	static constexpr Point delta(Direction dir) {
		/*
		// I wanna use C++14!
		switch(dir){
			case Direction::Up:    return Point(-1,  0);
			case Direction::Right: return Point( 0,  1);
			case Direction::Down:  return Point( 1,  0);
			case Direction::Left:  return Point( 0, -1);
		}
		*/
		return
		dir == Direction::Up ?    Point(-1,  0) : (
		dir == Direction::Right ? Point( 0,  1) : (
		dir == Direction::Down ?  Point( 1,  0) : 
								  Point( 0, -1)
		));
	}

	friend std::ostream& operator<<(std::ostream& out, const Point& p) {
		return out << '(' << p.x << ',' << p.y << ')';
	}


    /**************************************************************************
     * Rotation and flip
     *************************************************************************/

	constexpr Point rotateCW(int H) const {
		return Point(x, H-y-1);
	}

	constexpr Point rotateCCW(int W) const {
		return Point(W-x-1, y);
	}

	constexpr Point opposite(int H, int W) const {
		return Point(H-y-1, W-x-1);
	}

	constexpr Point flipY(int H) const {
		return Point(H-y-1, x);
	}

	constexpr Point flipX(int W) const {
		return Point(y, W-x-1);
	}

	Point rotateFlip(Direction topSide, bool yflip, bool xflip, int H, int W) const {
		Point ret = *this;
		if(yflip) ret = ret.flipY(H);
		if(xflip) ret = ret.flipX(W);
		if(topSide == Direction::Up)    return ret;
		if(topSide == Direction::Right) return rotateCCW(W);
		if(topSide == Direction::Down)  return opposite(H, W);
		return rotateCW(H);
	}
};

} // end of namespace slide

#ifdef __gcc__
#pragma GCC diagnostic pop
#endif

#endif
