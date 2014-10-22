#include <string>
#include <utility>

#include "Solver.hpp"
#include "DolphinSolver.hpp"
#include "DragonSolver.hpp"
#include "ExactSolver.hpp"
#include "HitodeSolver.hpp"
#include "KurageSolver.hpp"
#include "SharkSolver.hpp"
#include "StraightSolver.hpp"

namespace slide
{

std::unique_ptr<Solver> Solver::create_solver(const std::string& solver_name,
                                              const Problem& problem)
{
    if(solver_name == "straight"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::StraightSolver(problem)));
    }else if(solver_name == "exact"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::ExactSolver(problem)));
    }else if(solver_name == "hitode"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::HitodeSolver(problem)));
    }else if(solver_name == "kurage"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::KurageSolver(problem)));
    }else if(solver_name == "shark"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::SharkSolver(problem)));
    }else if(solver_name == "dolphin"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::DolphinSolver(problem)));
    }else if(solver_name == "dragon"){
        return std::move(std::unique_ptr<slide::Solver>(new slide::DragonSolver(problem)));
    }

    return nullptr;
}

} // end of namespace slide
