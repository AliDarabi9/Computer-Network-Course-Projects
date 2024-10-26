#include "send.h"

send::send(int port, QVector<QBuffer *> *packetBuff, const std::string &ip, QObject *parent)
    : QObject{parent}
{
    packetBuff_ = packetBuff;
    io_service_ = new boost::asio::io_service();
    socket_ = new boost::asio::ip::tcp::socket(*io_service_);
    bool timeOutFlag = true;
    for (int i = 0; i < 100; i++) {
        try {
            socket_->connect(
                boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip),
                                               port));
            timeOutFlag = false;
            break;
        } catch (...) {
            QThread::sleep(1);
        }
    }
    if (timeOutFlag) {
        qDebug() << "Connection Time Out !";
    }
    ip_ = ip;
}

void send::startSending()
{
    QByteArray data;
    if (packetBuff_->size() > 0) {
        QBuffer* sendBuffer = packetBuff_->first();
        data = sendBuffer->data();
        packetBuff_->pop_front();
        delete sendBuffer;
    } else {
        data = "";
    }
    boost::asio::write(*socket_, boost::asio::buffer(data + '\n'));
}
