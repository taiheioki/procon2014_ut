#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "network/ProblemClient.hpp"
#include "slide/Answer.hpp"
#include "slide/Solver.hpp"
#include "slide/Problem.hpp"

void callback(std::stringstream& ss)
{
    std::cout << "callback function" << std::endl;
    try{
        slide::Problem problem(ss);
        std::unique_ptr<slide::Solver> slide_solver = slide::Solver::create_solver("straight", problem);
        slide_solver->onCreatedAnswer = [&slide_solver](const slide::Answer& answer){
            const int numSelect = std::count_if(answer.begin(), answer.end(),
                                        [](slide::Move move){ return move.isSelection; });
            const int numSwap   = answer.size() - numSelect;
            const int cost      = numSelect * slide_solver->problem.selectionCost
                                  + numSwap * slide_solver->problem.swappingCost;

            std::cout << "select = " << numSelect << ", swap = " << numSwap
                      << ", cost = " << cost << std::endl;
            std::cout << answer << std::endl;
            std::cout << std::endl;
        };
        slide_solver->solve();
    }catch(std::exception& e){
        std::cerr << "[problem_client::callback] Error:"
                  << e.what() << std::endl;
    }
}



int main(int argc, char const* argv[])
{
    using boost::asio::ip::tcp;

    if(argc != 4){
        std::cerr << "Usage: " << argv[0] << " <host> <port> <solver>" << std::endl;
        return 1;
    }

    std::string solver(argv[3]);
    if(solver != "straight" && solver != "exact" &&
       solver != "hitode" && solver != "kurage" &&
       solver != "shark" && solver != "dolphin" &&
       solver != "dragon"){
        std::cerr << "invalid solver name: " << solver << std::endl;
        return 1;
    }

    try{
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        network::ProblemClient c(io_service, iterator, callback);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string s;
        while(true){
            std::cin >> s;
            if(s == "end"){
                std::cerr << "close connection" << std::endl;
                c.close();
                break;
            }
        }

        t.join();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
