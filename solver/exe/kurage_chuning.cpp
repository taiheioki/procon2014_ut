#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <unordered_set>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <tbb/parallel_for.h>

#include "slide/KurageBoard.hpp"
#include "slide/KurageSolver.hpp"

#include "util/define.hpp"
#include "util/Random.hpp"
#include "util/StopWatch.hpp"


struct Config
{
    std::size_t H;
    std::size_t W;
    int selectionLimit;
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
        ("max_selection_limit,l", po::value<int>(&config.selectionLimit)->default_value(16), "maximum limit time of the selection")
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

slide::Problem setProblem(int selectionLimit)
{
    const int swappingCost   = util::Random::nextInt(MIN_SWAPPING_COST,   MAX_SWAPPING_COST);
    const int selectionCost  = util::Random::nextInt(MIN_SELECTION_COST,  MAX_SELECTION_COST);

    slide::Problem problem(8, 8, swappingCost, selectionCost, selectionLimit);
    problem.board = slide::Board<slide::Flexible>::randomState(8, 8);

    return problem;
}

double measure()
{
    std::atomic_int sum(0);
    std::atomic_int cnt(0);

    tbb::parallel_for(0, 100, [&](int){
        bool solved = false;
        slide::KurageSolver solver(setProblem(2));
        solver.retry = false;

        solver.onCreatedAnswer = [&](const slide::Answer& answer){
            if(!solved){
                sum += answer.size();
                ++cnt;
                std::cerr << answer.size() << ' ';
                solved = true;
            }
        };

        solver.solve();
        if(!solved){
            std::cerr << "! ";
        }
    });

    const double score = double(sum.load()) / cnt.load();
    std::cerr << "\navg = " << score << std::endl;
    return score;
}

int main(int argc, const char* const argv[])
{
    using KurageBoard = slide::KurageBoard<8, 8>;

    const Config config = parseCommand(argc, argv);
    slide::KurageSolver::verbose = false;

    const float coef[5] = {15.0f, 1.0f, 30.0f, 12.0f, 2.0f};
    std::copy_n(coef, 5, KurageBoard::coefficients);

    for(int c=0;; ++c){
        double preScore = 1e100;
        double increment = true;

        for(int r=0; r<20; ++r){

            const int param_id = 3 - c % 4;
            const float pre = KurageBoard::coefficients[param_id];
            KurageBoard::coefficients[param_id] *= (increment ? 1.5f : 0.7f) * (20-r) / 20.0;

            for(float c : KurageBoard::coefficients){
                std::cout << c << ", ";
            }
            std::cout << std::endl;

            const double score = measure();
            if(score > preScore){
                KurageBoard::coefficients[param_id] = pre;
                increment = !increment;
            }
            else{
                preScore = score;
            }
            std::cerr << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
