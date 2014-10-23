#ifndef GETPROBLEMWORKER_HPP
#define GETPROBLEMWORKER_HPP

#include <QObject>
#include <QString>

#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "network/ProblemClient.hpp"

namespace slide{
namespace ui{

class GetProblemWorker : public QObject
{
    Q_OBJECT
public:
    GetProblemWorker(const std::string& addr, const std::string& port,
                     bool& err, QObject *parent = 0);
    ~GetProblemWorker();
    void stop();

public slots:
    void start();

signals:
    void gotProblem(const QString problem);


private:
    boost::asio::io_service io_service;
    std::unique_ptr<network::ProblemClient> client;
    std::unique_ptr<boost::asio::io_service::work> work;
};

} // end of namespace ui
} // end of namespace slide

#endif // GETPROBLEMWORKER_HPP
