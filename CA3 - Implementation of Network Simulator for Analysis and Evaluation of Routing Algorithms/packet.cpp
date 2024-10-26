#include "packet.h"

packet::packet(QString inMessage, int sourceId, int destId, QObject *parent)
    : QObject{parent}
{
    this->sourceID = sourceId;
    this->sourceIP = "192.168.0." + QString::number(this->sourceID);
    this->destID = destId;
    this->destIP = "192.168.0." + QString::number(this->destID);
    this->message = inMessage;
    this->cycleInBuffer = 0;
}

void packet::increaseBufferTime() {
    this->cycleInBuffer++;
}

packet* packet::decode(const QString &line) {
    QStringList parts = line.split("#");
    packet *newPacket = new packet(parts[3], parts[2].toInt(), parts[1].toInt());
    QStringList path = parts[4].split(",");
    for (const QString &str : path) {
        newPacket->routersInPath.push_back(str);
    }
    newPacket->cycleInBuffer = parts[5].toInt();
    return newPacket;
}

void packet::addRouterToPath(int id) {
    this->routersInPath.push_back(QString::number(id));
}

QString packet::incode(const packet &inPacket) {
    QString routersPath = inPacket.routersInPath.join(",");
    QString data = QString(MESSAGE) + "#" + QString::number(inPacket.destID) + "#" + QString::number(inPacket.sourceID) + "#" + inPacket.message + "#" + routersPath + "#" + QString::number(inPacket.cycleInBuffer);
    return data;
}

QString packet::printPacket() {
    routersInPath.removeFirst();
    QString routersPath = routersInPath.join(",");
    return "Packet Info: SourceIP:" + sourceIP
           + " DestIP:" + destIP
           + " Message:" + message
           + " RoutersInPath:" + routersPath
           + " PathLength:" + QString::number(routersInPath.size() + 1)
           + " CyclesInBuffer:" + QString::number(cycleInBuffer);
}
