#include <cstdlib>

#include <deque>
#include <iostream>
#include <string>
#include <utility>

#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Message.hpp"
#include "PostSession.hpp"
#include "slide/Answer.hpp"

namespace network{

using boost::asio::ip::tcp;

void PostSession::start()
{
    std::cerr << "[PostSession::start] (" << id << ") session start" << std::endl;
    manager.add_session(shared_from_this());
    do_read_header();
}

void PostSession::send_message(const Message /*msg*/)
{
}

void PostSession::do_read_header()
{
    std::cerr << "[PostSession::start] (" << id << ") read header" << std::endl;
    auto self(shared_from_this());
    boost::asio::async_read(socket,
        boost::asio::buffer(&msg_header[0], Message::header_length),
        [this,self](boost::system::error_code error, std::size_t length)
        {
            if(!error){
                msg_header[Message::header_length] = '\0';
                std::size_t len = (std::size_t)std::atoi(msg_header);
                std::cerr << "[PostSession::handle_do_read_header] (" << id << "): "
                          << length << "," << len << std::endl;
                delete read_msg;
                read_msg = new Message(len);
                do_read_body();
            }else{
                std::cerr << "[PostSession::do_read_header] Error:" << std::endl;
                std::cerr << error.message() << std::endl;
            }
        });
}

void PostSession::do_read_body()
{
    std::cerr << "[PostSession::start] (" << id << ") read body" << std::endl;
    auto self(shared_from_this());
    boost::asio::async_read(socket,
        boost::asio::buffer(read_msg->body(), read_msg->body_length()),
        [this,self](boost::system::error_code error, std::size_t length)
        {
            if(!error){
                //std::cout.write(read_msg->body(), read_msg->body_length());
                //std::cout << std::endl;
                std::cerr << "[PostSession::handle_do_read_body] (" << id << "): " << length << std::endl;
                std::stringstream ss;
                ss.rdbuf()->sputn(read_msg->body(), read_msg->body_length());
                callback(ss);
                do_read_header();
            }else{
                std::cerr << "[PostSession::do_read_body] Error:" << std::endl;
                std::cerr << error.message() << std::endl;
            }
        });
}

void PostSession::close()
{
    socket.close();
}

} // end of namespace network
