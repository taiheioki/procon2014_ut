#ifndef SLIDE_ANSWER_FEATURE_HPP_
#define SLIDE_ANSWER_FEATURE_HPP_

#include "Answer.hpp"
#include "PlayBoard.hpp"

namespace slide
{

class AnswerFeature
{
public:
	Answer answer;

	void init() {
		answer.clear();
	}

	void move(Direction dir) {
		answer.emplace_back(dir);
	}

	void select(Point newSelect) {
		answer.emplace_back(newSelect);
	}
};

} // end of namespace slide

#endif
