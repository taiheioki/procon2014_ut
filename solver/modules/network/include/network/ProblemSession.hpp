#ifndef NETWORK_PROBLEMSESSION_HPP
#define NETWORK_PROBLEMSESSION_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Message.hpp"
#include "SessionManager.hpp"

namespace network{

using boost::asio::ip::tcp;

class ProblemSession : public Session,
                       public std::enable_shared_from_this<ProblemSession>
{
public:
    ProblemSession(tcp::socket socket_, SessionManager& manager_, std::string id_) :
        socket(std::move(socket_)), manager(manager_)
    {
        id = id_;
    }

    void start();
    void send_message(const Message msg);
    void close();

private:
    void do_write();

    tcp::socket socket;
    SessionManager& manager;
    std::deque<Message> write_msgs;
};

} // end of namespace network

#endif
