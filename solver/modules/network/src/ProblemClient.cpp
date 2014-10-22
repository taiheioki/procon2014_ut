#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <streambuf>
#include <sstream>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Message.hpp"
#include "ProblemClient.hpp"
#include "slide/Problem.hpp"
#include "slide/Solver.hpp"

namespace network{

using boost::asio::ip::tcp;

ProblemClient::ProblemClient(boost::asio::io_service& io_service_,
                             const tcp::resolver::iterator& endpoint_iterator,
                             std::function<void(std::stringstream&)>callback_) :
   callback(callback_), io_service(io_service_), socket(io_service), read_msg(nullptr)
{
    std::cerr << "[ProblemClient::ProblemClient] start async_connect" << std::endl;
    boost::asio::async_connect(socket, endpoint_iterator,
            boost::bind(&ProblemClient::handle_connect, this,
                        boost::asio::placeholders::error));
}

void ProblemClient::close()
{
    io_service.post(boost::bind(&ProblemClient::do_close, this));
}

void ProblemClient::handle_connect(const boost::system::error_code& error)
{
    if(!error){
        std::string addr = socket.remote_endpoint().address().to_string();
        std::cerr << "[ProblemClient::handle_connect] connected: "
                  << addr << std::endl;
        boost::asio::async_read(socket,
            boost::asio::buffer(&msg_header, Message::header_length),
            boost::bind(&ProblemClient::handle_read_header, this,
                        boost::asio::placeholders::error));
    }else{
        std::cerr << "[ProblemClient::handle_connect] Error:"
                  << error.message() << std::endl;
        close();
    }
}

void ProblemClient::handle_read_header(const boost::system::error_code& error)
{
    if(!error){
        msg_header[Message::header_length] = '\0';
        std::size_t len = (std::size_t)std::atoi(msg_header);
        delete read_msg;
        read_msg = new Message(len);
        boost::asio::async_read(socket,
             boost::asio::buffer(read_msg->body(), read_msg->body_length()),
             boost::bind(&ProblemClient::handle_read_body, this,
                         boost::asio::placeholders::error));
    }else{
        std::cerr << "[ProblemClient::handle_read_header] Error:"
                  << error.message() << std::endl;
    }
}

void ProblemClient::handle_read_body(const boost::system::error_code& error)
{
    if(!error){
        std::stringstream ss;
        ss.rdbuf()->sputn(read_msg->body(), read_msg->body_length());
        callback(ss);
        std::cerr << "[ProblemClient::handle_read_body] restart read header" << std::endl;
        boost::asio::async_read(socket,
            boost::asio::buffer(&msg_header, Message::header_length),
            boost::bind(&ProblemClient::handle_read_header, this,
                        boost::asio::placeholders::error));
    }else{
        std::cerr << "[ProblemClient::handle_read_body] Error:"
                  << error.message() << std::endl;
    }
}

void ProblemClient::do_close()
{
    socket.close();
}

} //  end of namespace network

