#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <tbb/parallel_for.h>

#include "slide/ExactSolver.hpp"
#include "slide/HitodeSolver.hpp"
#include "slide/StraightSolver.hpp"
#include "slide/KurageSolver.hpp"
#include "slide/DolphinSolver.hpp"
#include "slide/DragonSolver.hpp"
#include "slide/LizardSolver.hpp"

#include "util/define.hpp"
#include "util/Random.hpp"
#include "util/StopWatch.hpp"


struct Config
{
    std::size_t H;
    std::size_t W;
    int selectionLimit;
    std::string solver;
    bool verbose;
};

Config parseCommand(int argc, const char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("Allowed options");
    Config config;

    opt.add_options()
        ("help",                                                                             "print this help message")
        ("height,h",              po::value<std::size_t>(&config.H)->default_value(4u),      "height of the board")
        ("width,w",               po::value<std::size_t>(&config.W)->default_value(4u),      "width of the board")
        ("selection_limit,l",     po::value<int>(&config.selectionLimit)->default_value(16), "limit of the selection")
        ("solver,s",              po::value<std::string>(&config.solver)->required(),        "solver to be used")
        ("verbose,v",                                                                        "A lot printing")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opt), vm);
    po::notify(vm);

    // show help
    if(vm.count("help")){
        std::cerr << opt << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    config.verbose = vm.count("verbose");
    return config;
}

std::mt19937 engine;
slide::Problem setProblem(const Config& config)
{
    const int swappingCost   = util::Random::nextInt(MIN_SWAPPING_COST,   MAX_SWAPPING_COST);
    const int selectionCost  = util::Random::nextInt(MIN_SELECTION_COST,  MAX_SELECTION_COST);
    slide::Problem problem(config.H, config.W, swappingCost, selectionCost, config.selectionLimit);
    problem.board = slide::Board<slide::Flexible>::randomState(config.H, config.W);

    return problem;
}

int sum;
int cnt;
double elapsed;
util::SpinMutex mutex;

void run(slide::Solver&& solver)
{
    util::StopWatch sw;
    int nowSize;
    double nowTime;

    slide::Problem problem = solver.problem;
    solver.onCreatedAnswer = [&](const slide::Answer& answer){
        nowSize = answer.size();
        nowTime = sw.elapsed_ms();
    };

    sw.start();
    solver.solve();

    std::lock_guard<util::SpinMutex> lock(mutex);
    ++cnt;
    sum += nowSize;
    elapsed += nowTime;

    std::cerr << boost::format("%3d: now swap = %4d, not time = %.1f, average swap = %.1f, average time = %.1f") 
        % cnt % nowSize % nowTime % (double(sum)/cnt) % (elapsed/cnt) << std::endl;
}

void solve(const Config& config)
{
    const slide::Problem problem = setProblem(config);

    if(config.solver == "straight"){
        run(slide::StraightSolver(problem));
    }
    else if(config.solver == "exact"){
        run(slide::ExactSolver(problem));
    }
    else if(config.solver == "hitode"){
        run(slide::HitodeSolver(problem));
    }
    else if(config.solver == "kurage"){
        slide::KurageSolver::verbose = config.verbose;
        run(slide::KurageSolver(problem));
    }
    else if(config.solver == "dolphin"){
        slide::DolphinSolver::verbose = config.verbose;
        slide::DolphinSolver solver(problem);
        run(std::move(solver));
    }
    else if(config.solver == "dragon"){
        slide::DragonSolver::verbose = config.verbose;
        run(slide::DragonSolver(problem));
    }
    else if(config.solver == "lizard"){
        slide::LizardSolver::verbose = config.verbose;
        run(slide::LizardSolver(problem));
    }
    else{
        throw std::runtime_error("unknown solver: " + config.solver);
    }
}

int main(int argc, const char* const argv[])
{
    const Config config = parseCommand(argc, argv);

    rep(i, 1000){
//    tbb::parallel_for(0, 100, [&](int){
        solve(config);
    }

    return EXIT_SUCCESS;
}
