#ifndef SEND_H
#define SEND_H

#include <QBuffer>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QVector>
#include <boost/asio.hpp>
#include <string>

class send : public QObject
{
    Q_OBJECT
public:
    explicit send(int port,
                  QVector<QBuffer *> *packetBuff,
                  const std::string &ip,
                  QObject *parent = nullptr);
    void startSending();
    boost::asio::io_service *io_service_;
    boost::asio::ip::tcp::socket *socket_;
    QVector<QBuffer *> *packetBuff_;

private:
    std::string ip_;

signals:
};

#endif // SEND_H
