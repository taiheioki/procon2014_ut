#ifndef NETWORK_PROBLECLIENT_HPP
#define NETWORK_PROBLECLIENT_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <sstream>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"
#include "slide/Problem.hpp"

namespace network{

using boost::asio::ip::tcp;

class ProblemClient
{
public:
    ProblemClient(boost::asio::io_service& io_service_,
                  const tcp::resolver::iterator& endpoint_iterator,
                  std::function<void(std::stringstream&)> callback);

    void close();
    std::function<void(std::stringstream&)> callback;
private:
    void handle_connect(const boost::system::error_code& error);
    void handle_read_header(const boost::system::error_code& error);
    void handle_read_body(const boost::system::error_code& error);
    void do_close();

    boost::asio::io_service& io_service;
    tcp::socket socket;
    std::string solver;
    char msg_header[Message::header_length+1];
    Message *read_msg;
};


} // end of namespace network

#endif
