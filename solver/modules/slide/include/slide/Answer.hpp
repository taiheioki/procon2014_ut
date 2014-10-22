#ifndef SLIDE_ANSWER_HPP_
#define SLIDE_ANSWER_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Direction.hpp"
#include "Point.hpp"
#include "util/define.hpp"

namespace slide
{

class Move
{
private:
	union {
		Direction direction;
		uchar selected;
	};

public:
	bool isSelection;

	Move() = default;
	constexpr Move(Direction direction) : direction(direction), isSelection(false) {}
	constexpr Move(Point selectedPoint) : selected(selectedPoint.toInt()), isSelection(true) {}

	Direction getDirection() const {
		BOOST_ASSERT(!isSelection);
		return direction;
	}
	Point getSelected() const {
		BOOST_ASSERT(isSelection);
		return Point(selected);
	}
};

class Answer : public std::vector<Move>
{
public:
	void optimize();
	friend std::ostream& operator<<(std::ostream& out, const Answer& ans);

	inline std::string toString() const {
		std::ostringstream sout;
		sout << *this;
		return sout.str();
	}
};

} // end of namespace slide

#endif
