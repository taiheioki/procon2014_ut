#ifndef NETWORK_POSTSERVER_HPP
#define NETWORK_POSTSERVER_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <functional>
#include <memory>
#include <set>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"

namespace network{

using boost::asio::ip::tcp;

class PostServer
{
public:
    PostServer(boost::asio::io_service& io_service_,
               const tcp::endpoint& endpoint,
               std::function<void(std::stringstream&)> callback);
    void close();
    std::function<void(std::stringstream&)> callback;

private:
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
