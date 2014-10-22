#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Message.hpp"
#include "SessionManager.hpp"
#include "ProblemSession.hpp"

namespace network{

using boost::asio::ip::tcp;

void ProblemSession::start()
{
    std::cerr << "[ProblemSession::start] (" << id << ") session start" << std::endl;
    manager.add_session(shared_from_this());
}

//void ProblemSession::send_message(const Message& msg)
void ProblemSession::send_message(const Message msg)
{
    std::cerr << "[ProblemSession::send_message] send" << std::endl;
    bool write_in_progress = !write_msgs.empty();
    std::cerr << "[ProblemSession::send_message] write in progress: "
              << write_in_progress << std::endl;
    write_msgs.push_back(msg);
    if(!write_in_progress){
        do_write();
    }
}

void ProblemSession::do_write()
{
    std::cerr << "[ProblemSession::do_write] async_write" << std::endl;
    std::cerr.write(write_msgs.front().body(), write_msgs.front().length());
    std::cerr << std::endl;
    auto self(shared_from_this());
    boost::asio::async_write(socket,
        boost::asio::buffer(write_msgs.front().data(),
                            write_msgs.front().length()),
        [this,self](boost::system::error_code error, std::size_t /*length*/)
        {
            if(!error){
                write_msgs.pop_front();
                if(!write_msgs.empty()){
                    do_write();
                }
            }else{
                std::cerr << "[ProblemSession::do_write] Error:" << std::endl;
                std::cerr << error.message() << std::endl;
                manager.remove_session(shared_from_this());
            }
        });
}

void ProblemSession::close()
{
    socket.close();
}

} //  end of namespace network

