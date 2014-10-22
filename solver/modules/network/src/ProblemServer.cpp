#include <cstdlib>

#include <deque>
#include <iostream>
#include <string>
#include <utility>

#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "ProblemServer.hpp"
#include "ProblemSession.hpp"
#include "slide/Problem.hpp"

namespace network{

using boost::asio::ip::tcp;

ProblemServer::ProblemServer(boost::asio::io_service& io_service_,
                             const tcp::endpoint& endpoint) :
    io_service(io_service_), acceptor(io_service, endpoint),
    socket(io_service), accept_cnt(0)
{
    do_accept();
}

void ProblemServer::send_problem(const slide::Problem& problem)
{
    std::string problem_message = problem.toString();
    Message msg(problem_message.c_str(), problem_message.length());

    io_service.post(boost::bind(&ProblemServer::do_send_message, this, msg));
}

void ProblemServer::send_problem(const std::string& problem)
{
    Message msg(problem.c_str(), problem.length());

    io_service.post(boost::bind(&ProblemServer::do_send_message, this, msg));
}

void ProblemServer::do_send_message(const Message msg)
{
    std::cerr << "[ProblemServer::do_send_problem] send" << std::endl;
    manager.send_message(msg);
}

void ProblemServer::do_accept()
{
    acceptor.async_accept(socket,
            boost::bind(&ProblemServer::handle_accept, this,
                        boost::asio::placeholders::error));
}

void ProblemServer::handle_accept(const boost::system::error_code& error)
{
    if(!error){
        std::string addr = socket.remote_endpoint().address().to_string();
        std::cerr << "[ProblemServer::handle_accept] accept from "
                  << addr << std::endl;
        std::string id = (boost::format("%s-%s") % addr % accept_cnt).str();
        accept_cnt += 1;
        std::make_shared<ProblemSession>(std::move(socket), manager, id)->start();
    }else{
        std::cerr << "[ProblemServer::handle_accept] Error:"
                  << error.message() << std::endl;
    }
    do_accept();
}

void ProblemServer::close()
{
    io_service.post(boost::bind(&ProblemServer::do_close, this));
}

void ProblemServer::do_close()
{
    std::cerr << "[ProblemServer::do_close] close" << std::endl;
    manager.close();
    socket.close();
}

} //  end of namespace network

