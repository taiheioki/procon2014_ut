#include <csignal>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include "network/PostServer.hpp"

void callback(std::stringstream& ss)
{
    std::cout << "[callback]" << std::endl;
    std::cout << ss.str() << std::endl;
}

int main(int argc, char const* argv[])
{
    using boost::asio::ip::tcp;

    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    try{
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
        network::PostServer server(io_service, endpoint, callback);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string s;
        while(true){
            std::cin >> s;
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
