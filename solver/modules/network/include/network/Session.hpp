#ifndef NETWORK_SESSION_HPP
#define NETWORK_SESSION_HPP

namespace network{

using boost::asio::ip::tcp;

class Session
{
    friend class SessionManager;
public:
    virtual ~Session() {}
    //virtual void send_message(const Message& msg) = 0;
    virtual void send_message(const Message msg) = 0;
    virtual void start() = 0;
    virtual void close() = 0;
    std::string id;
};

} // end of namespace network

#endif
