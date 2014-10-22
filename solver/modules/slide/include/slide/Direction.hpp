#ifndef SLIDE_DIRECTION_HPP_
#define SLIDE_DIRECTION_HPP_

#include <iostream>
#include "util/define.hpp"

namespace slide
{

class Direction;

class DirectionBase
{
private:
	uchar data;

	DirectionBase() = default;
	constexpr explicit DirectionBase(const uchar data) : data(data) {}

	friend Direction;

public:

	constexpr DirectionBase opposite() const {
		return DirectionBase(uchar(*this) ^ uchar(0x02));
	}

	constexpr DirectionBase cw() const {
		return *this + DirectionBase(uchar(0x01));
	}

	constexpr DirectionBase ccw() const {
		return *this + DirectionBase(uchar(0x03));
	}

	constexpr bool isRightOrLeft() const {
		return uchar(*this) & uchar(0x01);
	}

	constexpr bool isUpOrDown() const {
		return !isRightOrLeft();
	}

	constexpr DirectionBase operator+ (const DirectionBase& dir) const {
		return DirectionBase((uchar(*this) + dir) & uchar(0x03));
	}

	DirectionBase operator+=(const DirectionBase& dir) {
		return *this = operator+(dir);
	}

	constexpr DirectionBase operator- (const DirectionBase& dir) const {
		return DirectionBase((uchar(*this) + uchar(0x04) - dir) & uchar(0x03));
	}

	DirectionBase operator-=(const DirectionBase& dir) {
		return *this = operator-(dir);
	}

	constexpr operator uchar() const {
		return data;
	}

	friend std::ostream& operator<<(std::ostream& out, const DirectionBase& dir) {
		const char* const name[] = {"Up", "Right", "Down", "Left"};
		return out << name[uchar(dir)];
	}
};

class Direction : public DirectionBase
{
public:
	Direction() = default;
	constexpr explicit Direction(const uchar data) : DirectionBase(data) {}
	constexpr Direction(const DirectionBase data) : DirectionBase(data) {}

	static constexpr DirectionBase Up    {uchar(0x00)};
	static constexpr DirectionBase Right {uchar(0x01)};
	static constexpr DirectionBase Down  {uchar(0x02)};
	static constexpr DirectionBase Left  {uchar(0x03)};
};

} // end of namespace slide

#endif
