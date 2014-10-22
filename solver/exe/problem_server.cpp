#include <csignal>
#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include "network/ProblemServer.hpp"
#include "slide/Problem.hpp"

int main(int argc, char const* argv[])
{
    using boost::asio::ip::tcp;

    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    boost::filesystem::path path("resources/slide/4x4.txt");
    slide::Problem problem(path);

    try{
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
        network::ProblemServer server(io_service, endpoint);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string s;
        while(true){
            std::cin >> s;
            if(s == "send"){
                std::cerr << "send message" << std::endl;
                server.send_problem(problem);
            }else
            if(s == "end"){
                server.close();
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
