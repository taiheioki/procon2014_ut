#ifndef NETWORK_SESSIONMANAGER_HPP
#define NETWORK_SESSIONMANAGER_HPP

#include <cstdlib>

#include <iostream>
#include <deque>
#include <memory>
#include <set>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"
#include "Session.hpp"

namespace network{

using boost::asio::ip::tcp;
typedef std::shared_ptr<Session> session_ptr;

class SessionManager
{
public:
    void add_session(session_ptr session);
    void remove_session(session_ptr session);
    void send_message(const Message&);
    void close();

private:
    std::set<session_ptr> sessions;
};

} // end of namespace network

#endif
