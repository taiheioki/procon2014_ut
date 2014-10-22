#ifndef SLIDE_PROBLEM_HPP_
#define SLIDE_PROBLEM_HPP_

#include <iostream>
#include <sstream>
#include <utility>

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "Answer.hpp"
#include "Board.hpp"
#include "Point.hpp"
#include "util/define.hpp"

namespace slide
{

class Problem
{
public:
	Board<Flexible> board;
	int swappingCost;
	int selectionCost;
	int selectionLimit;

	inline Problem() = default;
	inline constexpr Problem(int h, int w, int swappingCost, int selectionCost, int selectionLimit)
		: board(h, w),
		  swappingCost(swappingCost),
		  selectionCost(selectionCost),
		  selectionLimit(selectionLimit) {}
		  
	inline Problem(const boost::filesystem::path& path) {
		load(path);
	}

	inline Problem(std::stringstream& ss) {
		parse(ss);
	}

	boost::optional<int> check(const Answer& answer) const;
	void load(const boost::filesystem::path& path);
	void parse(std::stringstream& ss);

    std::string toString() const;

	friend std::ostream& operator<<(std::ostream& out, const Problem& problem);
    friend bool operator==(const Problem& lhs, const Problem& rhs);
};

} // end of namespace slide

#endif
