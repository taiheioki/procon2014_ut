#ifndef POSTPROBLEMWORKER_HPP
#define POSTPROBLEMWORKER_HPP

#include <QObject>
#include <QString>

#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "slide/Answer.hpp"
#include "network/PostClient.hpp"

namespace slide{
namespace ui{

class PostProblemWorker : public QObject
{
    Q_OBJECT
public:
    PostProblemWorker(
            const std::string& addr, const std::string& port,
            bool& err, QObject *parent = 0);
    void stop();

signals:

public slots:
    void start();
    void send_answer(const slide::Answer answer);
    //void send_answer(const std::string answer);

private:
    boost::asio::io_service io_service;
    std::unique_ptr<network::PostClient> client;
    std::unique_ptr<boost::asio::io_service::work> work;
};

} // end of namespace ui
} // end of namespace slide

#endif // POSTPROBLEMWORKER_HPP
