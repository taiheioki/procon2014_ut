#ifndef NETWORK_POSTCLIENT_HPP
#define NETWORK_POSTCLIENT_HPP

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

#include "Message.hpp"
#include "slide/Answer.hpp"

namespace network{

using boost::asio::ip::tcp;

class PostClient
{
public:
    PostClient(boost::asio::io_service& io_service_,
               const tcp::resolver::iterator& endpoint_iterator);

    void send_answer(const slide::Answer ans);
    void send_answer(const std::string ans);
    void close();
private:
    void handle_connect(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error);
    void do_send_message(const Message& msg);
    void do_write();
    void do_close();

    boost::asio::io_service& io_service;
    tcp::socket socket;
    bool connected;
    std::string addr;
    std::deque<Message> write_msgs;
};


} // end of namespace network

#endif
