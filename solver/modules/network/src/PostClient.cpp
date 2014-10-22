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
#include "PostClient.hpp"
#include "slide/Answer.hpp"

namespace network{

using boost::asio::ip::tcp;

PostClient::PostClient(boost::asio::io_service& io_service_,
                       const tcp::resolver::iterator& endpoint_iterator
                       ) :
    io_service(io_service_), socket(io_service), connected(false)
{
    std::cerr << "[PostClient::PostClient] start async_connect" << std::endl;
    boost::asio::async_connect(socket, endpoint_iterator,
            boost::bind(&PostClient::handle_connect, this,
                        boost::asio::placeholders::error));
}

void PostClient::send_answer(const slide::Answer ans)
{
    if(!connected){
        std::cerr << "[PostClient::send_answer] client is not connected" << std::endl;
        return;
    }

    //std::stringstream ss;
    //ss << ans;
    //std::string ans_message = ss.str();
    std::string ans_message = ans.toString();
    std::cerr << "[PostClient::send_answer] send:" << ans_message.length() << std::endl;

    Message msg(ans_message.c_str(), ans_message.length());

    io_service.post(boost::bind(&PostClient::do_send_message, this, msg));
}

void PostClient::send_answer(const std::string ans)
{
    if(!connected){
        std::cerr << "[PostClient::send_answer] client is not connected" << std::endl;
        return;
    }

    std::cerr << "[PostClient::send_answer] send: " << ans.length() << std::endl;
    Message msg(ans.c_str(), ans.length());

    io_service.post(boost::bind(&PostClient::do_send_message, this, msg));
}

void PostClient::do_send_message(const Message& msg)
{
    std::cerr << "[PostClient::do_send_message] send: " << msg.length() << std::endl;
    bool write_in_progress = !write_msgs.empty();
    write_msgs.push_back(msg);
    if(!write_in_progress){
        do_write();
    }
}

void PostClient::do_write()
{
    std::cerr << "[PostClient::do_write] async_write: " << write_msgs.front().length() << std::endl;
    char header[9];
    std::memcpy(header, write_msgs.front().data(), 4);
    header[8] = '\0';
    size_t len = std::atoi(header);
    std::cerr << "[PostClient::do_write] header len: " << len << std::endl;
    boost::asio::async_write(socket,
        boost::asio::buffer(write_msgs.front().data(),
                            write_msgs.front().length()),
        boost::bind(&PostClient::handle_write, this,
                    boost::asio::placeholders::error));
}

void PostClient::handle_write(const boost::system::error_code& error)
{
    if(!error){
        std::cerr << "[PostClient::handle_write] successfully write" << std::endl;
        write_msgs.pop_front();
        if(!write_msgs.empty()){
            do_write();
        }
    }else{
        std::cerr << "[PostClient::do_write] Error:"
                  << error.message() << std::endl;
        close();
    }
}

void PostClient::close()
{
    std::cerr << "[PostClient::close] close connection " << addr << std::endl;
    io_service.post(boost::bind(&PostClient::do_close, this));
}

void PostClient::handle_connect(const boost::system::error_code& error)
{
    if(!error){
        connected = true;
        addr = socket.remote_endpoint().address().to_string();
        std::cerr << "[PostClient::handle_connect] connected: "
                  << addr << std::endl;
    }else{
        std::cerr << "[PostClient::handle_connect] Error:"
                  << error.message() << std::endl;
        close();
    }
}

void PostClient::do_close()
{
    connected = false;
    socket.close();
}

} //  end of namespace network

