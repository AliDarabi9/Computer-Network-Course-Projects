#include "port.h"

port::port(int number, int nodeID, uint idAS, QObject *parent)
    : QObject{parent}
{
    portNumber = number;
    _nodeID = nodeID;
    asID = idAS;
}

int port::getNodeID() {
    return _nodeID;
}

void port::send(QString data) {
    destination->buffer.push_back(data);
}

int port::getPortNumber() {
    return portNumber;
}

QString port::receive() {
    QString tempData = buffer.first();
    buffer.pop_front();
    return tempData;
}

void port::setPortDest(port* destPort) {
    destination = destPort;
}

bool port::isPortNumber(int number) {
    return portNumber == number;
}

bool port::isBufferEmpty() {
    return buffer.empty();
}
