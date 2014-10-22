#include <QString>

#include <cstdlib>
#include <iostream>
#include <string>
#include <memory>
#include <utility>

#include "slide/Answer.hpp"
#include "network/PostClient.hpp"
#include "postproblemworker.hpp"

namespace slide{
namespace ui{

using boost::asio::ip::tcp;

PostProblemWorker::PostProblemWorker(
        const std::string& addr, const std::string& port,
        bool& err, QObject *parent) :
    QObject(parent), client(nullptr), work(nullptr)
{
    try{
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(addr, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        client = std::move(std::unique_ptr<network::PostClient>(
                               new network::PostClient(io_service, iterator)));
    }catch(std::exception& e){
        std::cerr << "[GetProblemWorker::GetProblemWorker] Error: " << e.what() << std::endl;
        err = true;
        return;
    }
    err = false;
    return;
}

void PostProblemWorker::start()
{
    std::cerr << "[PostProblemWorker::start] start!" << std::endl;
    work = std::move(std::unique_ptr<boost::asio::io_service::work>
                     (new boost::asio::io_service::work(io_service)));
    io_service.run();
}

void PostProblemWorker::stop()
{
    std::cerr << "[PostProblemWorker::stop] stop!" << std::endl;
    io_service.stop();
}

//void PostProblemWorker::send_answer(const QString answer)
//{
//    client->send_answer(answer.toStdString());
//}

void PostProblemWorker::send_answer(const slide::Answer answer)
{
    std::cerr << "[PostProblemWorker::send_answer] send answer";
    client->send_answer(answer);
}

} // end of namespace ui
} // end of namespace slide
