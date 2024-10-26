#include "receive.h"

receive::receive(int port, QVector<QBuffer *> *reciveBuffer,
                 QObject *parent)
    : QObject{parent}
{
    reciveBuffer_ = reciveBuffer;
    boost::asio::io_service *io_service = new boost::asio::io_service;
    socket_ = new boost::asio::ip::tcp::socket(*io_service);
    acceptor_ = new boost::asio::ip::tcp::acceptor(
        *io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    acceptor_->accept(*socket_);
}

void receive::Start()
{
    std::string data;
    boost::asio::read_until(*socket_, boost::asio::dynamic_buffer(data), '\n');
    if (data[0] != '\n') {
        data = data.substr(0, data.length() - 1);
        QBuffer *newBuff = new QBuffer();
        newBuff->open(QIODevice::WriteOnly);
        newBuff->write(data.c_str());
        newBuff->close();
        reciveBuffer_->push_back(newBuff);
    }
    QTimer::singleShot(100, this,&receive::Start);
}
