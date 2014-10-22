#ifndef NETWORK_POSTSESSION_HPP
#define NETWORK_POSTSESSION_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <functional>
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

class PostSession : public Session,
                    public std::enable_shared_from_this<PostSession>
{
public:
    PostSession(tcp::socket socket_, SessionManager& manager_, std::string id_,
                std::function<void(std::stringstream&)>callback_) :
        callback(callback_), socket(std::move(socket_)), manager(manager_), read_msg(nullptr)
    {
        id = id_;
    }

    void start();
    void close();
    void send_message(const Message);

    std::function<void(std::stringstream&)> callback;

private:
    void do_read_header();
    void do_read_body();

    tcp::socket socket;
    SessionManager& manager;
    Message* read_msg;
    char msg_header[Message::header_length+1];
};

} // end of namespace network

#endif
