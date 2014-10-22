#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
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
typedef std::shared_ptr<Session> session_ptr;

void SessionManager::add_session(session_ptr session)
{
    if(sessions.find(session) != std::end(sessions)){
        std::cerr << "[SessionManager::add_session] Error: duplicated session: "
                  << session->id << std::endl;
        return;
    }
    std::cerr << "[SessionManager::add_session] added: "
              << session->id << std::endl;
    sessions.insert(session);
}

void SessionManager::remove_session(session_ptr session)
{
    std::cerr << "[SessionManager::remove_session] remove: "
              << session->id << std::endl;
    sessions.erase(session);
}

void SessionManager::send_message(const Message& msg)
{
    std::cerr << "[SessionManager::send_message] send: " << std::endl;
    //std::cerr.write(msg.body(), msg.body_length());
    //std::cerr << std::endl;
    for(auto session: sessions){
        session->send_message(msg);
    }
}

void SessionManager::close()
{
    for(auto session: sessions){
        session->close();
    }
    sessions.clear();
}


} //  end of namespace network

