#ifndef SLIDE_SOLVER_HPP_
#define SLIDE_SOLVER_HPP_

#include <functional>
#include <string>
#include <utility>

#include "Answer.hpp"
#include "Problem.hpp"

namespace slide
{

class Solver
{
public:
	Problem problem;
    int numThreads = -1;
	std::function<void(const Answer&)> onCreatedAnswer;

	Solver() = default;
	Solver(const Problem& problem) : problem(problem) {}
	virtual ~Solver() = default;

	virtual void solve() = 0;
	static std::unique_ptr<Solver> create_solver(const std::string& solver_name,
                                                 const Problem& problem);
};

} // end of namespace slide

#endif
