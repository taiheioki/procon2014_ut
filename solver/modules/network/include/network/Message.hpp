#ifndef NETWORK_MESSAGE_HPP
#define NETWORK_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace network{

class Message
{
public:
    Message() : data_(new char[header_length]), body_length_(0)
    {
        encode_header();
    }

    Message(const std::size_t length)
        : data_(new char[length + header_length]), body_length_(length)
    {
        encode_header();
    }

    Message(const char* msg, const std::size_t length) :
        body_length_(length)
    {
        data_ = new char[header_length + body_length_];
        encode_header();
        std::memcpy(body(), msg, body_length());
    }

    Message(const Message& msg)
        : data_(new char[msg.length()]), body_length_(msg.body_length())
    {
        std::memcpy(data_, msg.data(), msg.length());
    }

    Message(Message&& msg)
        : data_(msg.data()), body_length_(msg.body_length())
    {
        msg.data_ = nullptr;
        msg.body_length_ = 0;
    }

    ~Message(){
        delete data_;
    }

    void encode_header(){
        char header[header_length+1] = "";
        std::sprintf(header, "%8d", (int)body_length_);
        std::memcpy(data_, header, header_length);
    }

    bool decode_header(){
        char header[header_length+1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = (std::size_t)std::atoi(header);
        return true;
    }

    std::size_t body_length() const{
        return body_length_;
    }

    char* data(){
        return data_;
    }

    const char* data() const{
        return data_;
    }

    char* body(){
        return data_ + header_length;
    }

    const char* body() const{
        return data_ + header_length;
    }

    std::size_t length() const{
        return header_length + body_length_;
    }

    static const std::size_t header_length = 8;
private:
    char *data_;
    std::size_t body_length_;
};

} // end of namespace network

#endif
