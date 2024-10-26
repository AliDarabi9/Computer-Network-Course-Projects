#ifndef RECEIVE_H
#define RECEIVE_H

#include <QBuffer>
#include <QDebug>
#include <QObject>
#include <QVector>
#include <audioplay.h>
#include <boost/asio.hpp>

class receive : public QObject
{
    Q_OBJECT
public:
    explicit receive(int port, QVector<QBuffer *> *reciveBuffer,
                     QObject *parent = nullptr);
    void Start();
    boost::asio::ip::tcp::acceptor *acceptor_;
    boost::asio::ip::tcp::socket *socket_;
    QVector<QBuffer *> *reciveBuffer_;
signals:
    void finished();
};

#endif // RECEIVE_H
