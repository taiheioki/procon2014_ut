#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <stdexcept>
#include <thread>
#include <unordered_set>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "slide/ExactSolver.hpp"
#include "slide/HitodeSolver.hpp"
#include "slide/KurageSolver.hpp"
#include "slide/StraightSolver.hpp"
#include "slide/DragonSolver.hpp"

#include "util/define.hpp"
#include "util/Random.hpp"
#include "util/ThreadIndexManager.hpp"


struct Config
{
    int H;
    int W;
    int maxSelectionLimit;
    int timeOut;
    bool verbose;
    std::string solver;
};

Config parseCommand(int argc, const char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("Allowed options");
    Config config;

    opt.add_options()
        ("help",                                                                                                 "print this help message")
        ("height,h",              po::value<int>(&config.H)->default_value(-1),                          "maximum height of the board")
        ("width,w",               po::value<int>(&config.W)->default_value(-1),                          "maximum width of the board")
        ("max_selection_limit,l", po::value<int>(&config.maxSelectionLimit)->default_value(MAX_SELECTION_LIMIT), "maximum limit time of the selection")
        ("solver,s",              po::value<std::string>(&config.solver)->required(),                            "solver to be used")
        ("time_out,t",            po::value<int>(&config.timeOut)->default_value(60),                            "time out [sec]")
        ("verbose,v",                                                                                            "A lot printing")
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

slide::Problem setProblem(const Config& config)
{
    const int h = config.H == -1 ? util::Random::nextInt(MIN_DIVISION_NUM, MAX_DIVISION_NUM) : config.H;
    const int w = config.W == -1 ? util::Random::nextInt(MIN_DIVISION_NUM, MAX_DIVISION_NUM) : config.W;
    const int swappingCost   = util::Random::nextInt(MIN_SWAPPING_COST,   MAX_SWAPPING_COST);
    const int selectionCost  = util::Random::nextInt(MIN_SELECTION_COST,  MAX_SELECTION_COST);
    const int selectionLimit = util::Random::nextInt(MIN_SELECTION_LIMIT, config.maxSelectionLimit);

    slide::Problem problem(h, w, swappingCost, selectionCost, selectionLimit);
    problem.board = slide::Board<slide::Flexible>::randomState(h, w);
    return problem;
}

void run(slide::Solver&& solver, int timeOut)
{
    slide::Problem problem = solver.problem;

    solver.onCreatedAnswer = [&problem](const slide::Answer& answer){
        if(!problem.check(answer)){
            std::cerr << "the answer was wrong!!" << std::endl;
            std::cerr << problem << std::endl;
            std::cerr << answer << std::endl;
            for(slide::Move move : answer){
                (move.isSelection ? std::cerr << move.getSelected() : std::cerr << move.getDirection()) << " ";
            }
            std::terminate();
        }
    };

    std::timed_mutex mutex;
    std::atomic_bool flag(false);

    std::thread th([&]{
        std::lock_guard<std::timed_mutex> lock(mutex);
        flag.store(true);
        util::ThreadIndexManager::clear();
        solver.solve();
    });

    while(!flag.load());

    std::unique_lock<std::timed_mutex> lock(mutex, std::defer_lock);
    if(!lock.try_lock_for(std::chrono::seconds(timeOut))){
        std::cerr << "timed out!!" << std::endl;
        std::cerr << problem << std::endl;
        std::terminate();
    }

    th.join();
}

void solve(const Config& config)
{
    const slide::Problem problem = setProblem(config);

    if(config.solver == "straight"){
        run(slide::StraightSolver(problem), config.timeOut);
    }
    else if(config.solver == "exact"){
        run(slide::ExactSolver(problem), config.timeOut);
    }
    else if(config.solver == "hitode"){
        run(slide::HitodeSolver(problem), config.timeOut);
    }
    else if(config.solver == "kurage"){
        slide::KurageSolver::verbose = config.verbose;
        run(slide::KurageSolver(problem), config.timeOut);
    }
    else if(config.solver == "dragon"){
        slide::DragonSolver::verbose = config.verbose;
        run(slide::DragonSolver(problem), config.timeOut);
    }
    else{
        throw std::runtime_error("unknown solver: " + config.solver);
    }
}

int main(int argc, const char* const argv[])
{
    const Config config = parseCommand(argc, argv);

    for(int i=0;; ++i){
        solve(config);
        if((i+1) % 10 == 0){
            std::cerr << "solved " << i+1 << " problems successfully!" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
