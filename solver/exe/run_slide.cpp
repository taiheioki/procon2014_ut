#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "slide/ExactSolver.hpp"
#include "slide/HitodeSolver.hpp"
#include "slide/KurageSolver.hpp"
#include "slide/StraightSolver.hpp"
#include "slide/SharkSolver.hpp"
#include "slide/DolphinSolver.hpp"
#include "slide/DragonSolver.hpp"
#include "slide/LizardSolver.hpp"
#include "slide/L2Solver.hpp"

#include "util/Random.hpp"
#include "util/StopWatch.hpp"


struct Config
{
    std::size_t H;
    std::size_t W;
    int swappingCost;
    int selectionCost;
    int selectionLimit;
    std::string file;
    int practiceNo;
    bool verbose;
    bool outputAnswer;
    std::unordered_set<std::string> solvers;
};

Config parseCommand(int argc, const char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("Allowed options");
    Config config;

    opt.add_options()
        ("help",                                                                        "print this help message")
        ("height,h",          po::value<std::size_t>(&config.H)->default_value(4u),     "height of the board")
        ("width,w",           po::value<std::size_t>(&config.W)->default_value(4u),     "width of the board")
        ("swaping_cost,a",    po::value<int>(&config.swappingCost)->default_value(10),  "cost per move of the piece")
        ("selection_cost,b",  po::value<int>(&config.selectionCost)->default_value(20), "cost per select of the piece")
        ("selection_limit,l", po::value<int>(&config.selectionLimit)->default_value(5), "limit time of the selection")
        ("solver,s",          po::value<std::vector<std::string>>(),                    "solver to be used")
        ("file,f",            po::value<std::string>(&config.file)->default_value(""),  "input file")
        ("practice_no,p",     po::value<int>(&config.practiceNo)->default_value(-1),    "No of practice problem")
        ("verbose,v",                                                                   "A lot printing")
        ("output_answer,o",                                                             "Output answer")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opt), vm);
    po::notify(vm);

    // show help
    if(vm.count("help")){
        std::cerr << opt << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    if(vm.count("solver")){
        const std::vector<std::string>& solvers = vm["solver"].as<std::vector<std::string>>();
        std::for_each(solvers.begin(), solvers.end(), [&config](const std::string& solver){
            config.solvers.insert(solver);
        });
    }

    config.verbose      = vm.count("verbose");
    config.outputAnswer = vm.count("output_answer");
    return config;
}

slide::Problem setProblem(const Config& config)
{
    if(config.practiceNo != -1){
        const boost::filesystem::path path = (boost::format("../resources/answer/practice/%02d.txt") % config.practiceNo).str();
        std::cerr << "loading " << path << std::endl;
        return slide::Problem(path);
    }
    else if(!config.file.empty()){
        std::cerr << "loading " << config.file << std::endl;
        return slide::Problem(config.file);
    }
    else{
        slide::Problem problem(config.H, config.W, config.swappingCost, config.selectionCost, config.selectionLimit);
        problem.board = slide::Board<slide::Flexible>::randomState(config.H, config.W);
        return problem;
    }
}

void solve(slide::Solver&& solver, bool outputAnswer)
{
    util::StopWatch sw;

    solver.onCreatedAnswer = [&](const slide::Answer& answer){
        const int numSelect = std::count_if(answer.begin(), answer.end(), [](slide::Move move){ return move.isSelection; });
        const int numSwap   = answer.size() - numSelect;
        const int cost      = numSelect * solver.problem.selectionCost + numSwap * solver.problem.swappingCost;

        std::cout << "select = " << numSelect << ", swap = " << numSwap
                  << ", cost = " << cost << ", time = " << sw.elapsed_ms() << "ms, "
                  << (solver.problem.check(answer) ? "solved correctly." : "failed to solve!") << std::endl;

        if(outputAnswer){
            std::cout << answer << std::endl;
        }
    };

    sw.start();
    solver.solve();
    util::StopWatch::show();
}

int main(int argc, const char* const argv[])
{
    const Config config = parseCommand(argc, argv);
    const slide::Problem problem = setProblem(config);
    std::cout << problem << std::endl;

    if(config.solvers.empty() || config.solvers.count("straight")){
        std::cout << "straight : ";
        solve(slide::StraightSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("exact")){
        std::cout << "exact    : ";
        solve(slide::ExactSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("hitode")){
        std::cout << "hitode : ";
        solve(slide::HitodeSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("kurage")){
        std::cout << "kurage : ";
        slide::KurageSolver::verbose = config.verbose;
        solve(slide::KurageSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("shark")){
        std::cout << "shark : ";
        slide::SharkSolver::verbose = config.verbose;
        solve(slide::SharkSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("dolphin")){
        std::cout << "dolphin : ";
        slide::DolphinSolver::verbose = config.verbose;
        solve(slide::DolphinSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("dragon")){
        std::cout << "dragon : ";
        slide::DragonSolver::verbose = config.verbose;
        solve(slide::DragonSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("lizard")){
        std::cout << "lizard : ";
        slide::LizardSolver::verbose = config.verbose;
        solve(slide::LizardSolver(problem), config.outputAnswer);
    }

    if(config.solvers.empty() || config.solvers.count("L2")){
        std::cout << "L2 : ";
        slide::L2Solver::verbose = config.verbose;
        solve(slide::L2Solver(problem), config.outputAnswer);
    }

    return EXIT_SUCCESS;
}
