#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "network/PostClient.hpp"
#include "slide/Problem.hpp"
#include "slide/Answer.hpp"
#include "slide/Solver.hpp"

int main(int argc, char const* argv[])
{
    using boost::asio::ip::tcp;

    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    boost::filesystem::path path("resources/slide/4x4.txt");
    slide::Problem problem(path);
    std::unique_ptr<slide::Solver> solver = slide::Solver::create_solver("straight", problem);

    try{
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        network::PostClient c(io_service, iterator);

        boost::asio::io_service::work work(io_service);
        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string s;
        while(true){
            std::cin >> s;
            if(s == "send"){
                std::unique_ptr<slide::Solver> solver = slide::Solver::create_solver("straight", problem);
                solver->onCreatedAnswer = [&solver, &c](const slide::Answer& answer){
                    const int numSelect = std::count_if(answer.begin(), answer.end(),
                                                [](slide::Move move){ return move.isSelection; });
                    const int numSwap   = answer.size() - numSelect;
                    const int cost      = numSelect * solver->problem.selectionCost
                                          + numSwap * solver->problem.swappingCost;

                    std::cout << "select = " << numSelect << ", swap = " << numSwap
                              << ", cost = " << cost << std::endl;
                    std::cout << answer << std::endl;
                    std::cout << std::endl;
                    std::cout << "send answer" << std::endl;
                    c.send_answer(answer);
                };
                solver->solve();
            }else
            if(s == "end"){
                std::cerr << "close connection" << std::endl;
                c.close();
                break;
            }
        }

        io_service.stop();
        t.join();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
