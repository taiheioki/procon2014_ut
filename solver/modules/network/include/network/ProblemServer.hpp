#ifndef NETWORK_PROBLEMSERVER_HPP
#define NETWORK_PROBLEMSERVER_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <memory>
#include <set>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "slide/Problem.hpp"

namespace network{

using boost::asio::ip::tcp;

class ProblemServer
{
public:
    ProblemServer(boost::asio::io_service& io_service_,
                  const tcp::endpoint& endpoint);

    void send_problem(const slide::Problem& problem);
    void send_problem(const std::string& problem);
    void close();
private:
    void do_send_message(const Message msg);
    void do_accept();
    void do_close();
    void handle_accept(const boost::system::error_code& error);

    boost::asio::io_service& io_service;
    tcp::acceptor acceptor;
    tcp::socket socket;
    SessionManager manager;
    int accept_cnt;
};


} // end of namespace network

#endif
