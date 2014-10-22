#include <QString>

#include <memory>
#include <string>
#include <utility>

#include <boost/assert.hpp>
#include <boost/asio.hpp>

#include "getproblemworker.hpp"
#include "network/ProblemClient.hpp"

namespace slide{
namespace ui{

using boost::asio::ip::tcp;

GetProblemWorker::GetProblemWorker(
        const std::string& addr, const std::string& port,
        bool& err, QObject *parent) :
    QObject(parent), client(nullptr), work(nullptr)
{
    try{
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(addr, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        client = std::move(std::unique_ptr<network::ProblemClient>(
                               new network::ProblemClient(io_service, iterator,
                               [this](std::stringstream& ss) { emit gotProblem(QString::fromStdString(ss.str())); })));
    }catch(std::exception& e){
        std::cerr << "[GetProblemWorker::GetProblemWorker] Error: " << e.what() << std::endl;
        err = true;
        return;
    }
    err = false;
    return;
}

void GetProblemWorker::start()
{
    std::cerr << "[GetProblemWorker::start] start!" << std::endl;
    work = std::move(std::unique_ptr<boost::asio::io_service::work>
                     (new boost::asio::io_service::work(io_service)));
    io_service.run();
}

void GetProblemWorker::stop()
{
    std::cerr << "[GetProblemWorker::stop] stop!" << std::endl;
    io_service.stop();
}


GetProblemWorker::~GetProblemWorker()
{
    stop();
}

} // end of namespace ui
} // end of namespace slide
