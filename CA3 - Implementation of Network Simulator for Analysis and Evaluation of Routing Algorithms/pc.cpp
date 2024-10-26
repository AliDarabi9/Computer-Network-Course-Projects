#include "pc.h"

pc::pc(uint idAS, QObject *parent)
    : node{parent}
{
    asID = idAS;
    IP = "192.168.0." + QString::number(_id);
}

void pc::addNewPort(int portNumber) {
    sourcePort = new port(portNumber, _id, asID);
}

void pc::bindDestPort(int portNumber, port* destPort) {
    sourcePort->setPortDest(destPort);
}

port* pc::getPortByPortNumber(int portNumber) {
    return sourcePort;
}

void pc::startPCs(int destID) {
    packet p("HELLO", _id, destID);
    sourcePort->send(packet::incode(p));
}

void pc::update() {
    QString data;
    if (!sourcePort->isBufferEmpty()) {
        data = sourcePort->receive();
        packet* p = packet::decode(data);
        qDebug() << "PC:" << _id << "Received:" << p->printPacket();
        if (p != nullptr) {
            delete p;
        }
     }
}
