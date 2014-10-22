#include <cstdlib>

#include <deque>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "PostServer.hpp"
#include "PostSession.hpp"
#include "slide/Problem.hpp"

namespace network{

using boost::asio::ip::tcp;

PostServer::PostServer(boost::asio::io_service& io_service_,
                       const tcp::endpoint& endpoint,
                       std::function<void(std::stringstream&)>callback_) :
    callback(callback_), io_service(io_service_),
    acceptor(io_service, endpoint), socket(io_service), accept_cnt(0)
{
    do_accept();
}

void PostServer::do_accept()
{
    acceptor.async_accept(socket,
            boost::bind(&PostServer::handle_accept, this,
                        boost::asio::placeholders::error));
}

void PostServer::handle_accept(const boost::system::error_code& error)
{
    if(!error){
        std::string addr = socket.remote_endpoint().address().to_string();
        std::cerr << "[PostServer::handle_accept] accept from "
                  << addr << std::endl;
        std::string id = (boost::format("%s-%s") % addr % accept_cnt).str();
        accept_cnt += 1;
        std::make_shared<PostSession>(std::move(socket), manager, id, callback)->start();
    }else{
        std::cerr << "[PostServer::handle_accept] Error:"
                  << error.message() << std::endl;
    }
    do_accept();
}

void PostServer::close()
{
    io_service.post(boost::bind(&PostServer::do_close, this));
}


void PostServer::do_close()
{
    std::cerr << "[PostServer::do_close] close" << std::endl;
    manager.close();
    socket.close();
}

} // end of namespace network
