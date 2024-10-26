#include "router.h"

Router::Router(char protocol, uint idAS, QObject *parent)
    : node(parent)
{
    ProtocolType = protocol;
    asID = idAS;
    IP = "192.168.0." + QString::number(_id);
 }

void Router::send(QString data, port* p, char messageType) {
    if (messageType == EBGP) {
        data = QString(messageType) + "#" + data;
    }
    p->send(data);
}

void Router::addNewPort(int portNumber) {
    ports.push_back(new port(portNumber, _id, asID));
}

void Router::bindDestPort(int portNumber, port* destPort) {
    port* p = getPortByPortNumber(portNumber);
    if (p != nullptr) {
        p -> setPortDest(destPort);
        int id = destPort->getNodeID();
        lsdb.table[id] = 1;
        distanceVector.table[id] = 1;
        forwardingTable[id] = portNumber;
    }
}

void Router::startOSPF() {
    lsdb.visitedNodeIDs.clear();
    lsdb.visitedNodeIDs.push_back(_id);
    isStartOSPF = true;
    lspIndex = 0;
    sendOSPF = true;
    sendLsp();
}

void Router::startRIP() {
    this->isRIPstart = true;
    this->ripFinished = false;
    this->ripConvergenceCondition = 0;
    this->ripFinishSignalEmited = false;
    sendRip();
}

void Router::startProtocol(uint idAS) {
    if (asID == idAS) {
        if (ProtocolType == OSPF) {
            startOSPF();
        }
        else if (ProtocolType == RIP) {
            startRIP();
        }
        else {
            qDebug() << "NO MATCHING PROTOCOL.";
        }
    }
}

bool Router::isNodeVisited(int nodeID) {
    for (int id : lsdb.visitedNodeIDs) {
        if (nodeID == id) {
            return true;
        }
    }
    return false;
}

void Router::sendLsp() {
    port* p = nullptr;
    if (lspIndex < ports.size()) {
        p = ports[lspIndex];
    }
    QString data = lsp::incode(lsdb);
    if (p == nullptr) {
        if (!isStartOSPF) {
            lspBackTrack();
        }
        else {
            handleOSPFFinish();
        }
    } else if (!isNodeVisited(p->destination->getNodeID()) && (p->destination->asID == asID)) {
        send(data, p, PROTOCOL);
    } else {
        lspIndex++;
        sendLsp();
    }
}

port* Router::getPortByPortNumber(int portNumber) {
    for (port* p : ports) {
        if (p->isPortNumber(portNumber)) {
            return p;
        }
    }
    return nullptr;
}

void Router::forward(QString data) {
    packet* inPacket = packet::decode(data);
    int portNumber = forwardingTable[inPacket->destID];
    port* sendPort = getPortByPortNumber(portNumber);
    if (sendPort != nullptr) {
        inPacket->addRouterToPath(_id);
        send(packet::incode(*inPacket), sendPort, MESSAGE);
    }
    if (inPacket != nullptr) {
        delete inPacket;
    }
}

void Router::updateLsp(lsp* newData, int portNumber) {
    QList<int> keys  = newData->table.keys();
    for (int key : keys) {
        if (key == _id) {
            continue;
        }
        if (lsdb.table.contains(key)) {
            if (lsdb.table[key] > newData->table[key] + 1) {
                lsdb.table[key] = newData->table[key] + 1;
                forwardingTable[key] = portNumber;
            }
        } else {
            lsdb.table[key] = newData->table[key] + 1;
            forwardingTable[key] = portNumber;
        }
    }
}

void Router::lspBackTrack() {
    sendOSPF = false;
    QString data = lsp::incode(lsdb);
    send(data, lspReceivePort, PROTOCOL);
}

void Router::handleOSPFFinish() {
    sendOSPF = false;
    isStartOSPF = false;
    emit OSPFFinished();
    qDebug() << "Router:" << _id << "OSPF finished.";
}

void Router::updateOSPF() {
    QString data;
    if (!sendOSPF) {
        for (port* p : ports) {
            if (!p->isBufferEmpty()) {
                data = p->receive();
                lsdb.visitedNodeIDs.clear();
                lspIndex = 0;
                lspReceivePort = p;
                sendOSPF = true;
                lsdb.visitedNodeIDs = lsp::decode(data)->visitedNodeIDs;
                lsdb.visitedNodeIDs.push_back(_id);
                if (ports.size() > 1) {
                    sendLsp();
                }
                else {
                    lspBackTrack();
                }
            }
        }
    }
    else {
        port* sendPort = ports[lspIndex];
        if (!sendPort->isBufferEmpty()) {
            data = sendPort->receive();
            lspIndex++;
            lsp* newData = lsp::decode(data);
            updateLsp(newData, sendPort->getPortNumber());
            if (newData != nullptr) {
                delete newData;
            }
            if (isStartOSPF) {
                if (lspIndex >= ports.size()){
                    handleOSPFFinish();
                }
                else {
                    sendLsp();
                }
            }
            else if (lspIndex >= (ports.size() - 1)){
                lspBackTrack();
            }
            else {
                sendLsp();
            }
        }
    }
}

bool Router::updateDistanceVector(rip* newData, int portNumber) {
    QList<int> keys  = newData->table.keys();
    bool isAnythingChanged = false;
    for (int key : keys) {
        if (key == _id) {
            continue;
        }
        if (distanceVector.table.contains(key)) {
            if (distanceVector.table[key] > newData->table[key] + 1) {
                distanceVector.table[key] = newData->table[key] + 1;
                forwardingTable[key] = portNumber;
                isAnythingChanged = true;
            }
        } else {
            distanceVector.table[key] = newData->table[key] + 1;
            forwardingTable[key] = portNumber;
            isAnythingChanged = true;
        }
    }
    return isAnythingChanged;
}

void Router::sendRip() {
    QString data = rip::incode(distanceVector);
    for (port* p : ports) {
        if (p->destination->asID == asID) {
            send(data, p, PROTOCOL);
        }
    }
}

void Router::updateRIP() {
    QString data;
    bool isAnyPacketReceive = false;
    bool isAnythingChanged = false;
    for (port* neighbour : ports) {
        if (!neighbour->isBufferEmpty()) {
            data = neighbour->receive();
            isAnyPacketReceive = true;
            rip* newData = rip::decode(data);
            if (updateDistanceVector(newData, neighbour->getPortNumber())) {
                isAnythingChanged = true;
            }
            if (newData != nullptr) {
                delete newData;
            }
        }
    }
    if (isAnyPacketReceive && !isAnythingChanged) {
        ripConvergenceCondition++;
    }
    if (ripConvergenceCondition >= 5) {
        ripFinished = true;
    }
    if (!ripFinished) {
        sendRip();
    }
    else {
        if (!ripFinishSignalEmited) {
            emit RIPFinished();
            this->ripFinishSignalEmited = true;
            this->isRIPstart = false;
        }
    }
}

void Router::printTable() {
    QHash<int, int> table = (ProtocolType == OSPF) ? lsdb.table : distanceVector.table;
    QString output;
    QTextStream stream(&output);
    stream << "Forwarding Table for Router " << _id << ":\n";
    stream << qSetFieldWidth(10) << "IP" << qSetFieldWidth(0) << " | "
           << qSetFieldWidth(10) << "Port" << qSetFieldWidth(0) << "\n";
    stream << "-----------------------\n";
    for (auto it = forwardingTable.constBegin(); it != forwardingTable.constEnd(); ++it) {
        stream << qSetFieldWidth(10) << "192.168.0." + QString::number(it.key()) << qSetFieldWidth(0) << " | "
               << qSetFieldWidth(5) << it.value() << qSetFieldWidth(0) << "\n";
    }
    stream << "\nTable Cost for Router " << _id << ":\n";
    stream << qSetFieldWidth(10) << "IP" << qSetFieldWidth(0) << " | "
           << qSetFieldWidth(10) << "Cost" << qSetFieldWidth(0) << "\n";
    stream << "-----------------------\n";
    for (auto it = table.constBegin(); it != table.constEnd(); ++it) {
        stream << qSetFieldWidth(10) << "192.168.0." + QString::number(it.key()) << qSetFieldWidth(0) << " | "
               << qSetFieldWidth(5) << it.value() << qSetFieldWidth(0) << "\n";
    }
    qDebug().noquote() << output;
}

void Router::increaseTimeInBuffer() {
    for (port* p : ports) {
        for (QString& str : p->buffer) {
            QStringList parts = str.split('#');
            if (parts[0] == MESSAGE) {
                packet* p = packet::decode(str);
                p->increaseBufferTime();
                str = packet::incode(*p);
                if (p != nullptr) {
                    delete p;
                }
            }
        }
    }
}

void Router::update() {

    if ((ProtocolType == OSPF) && isOSPFstart) {
        updateOSPF();
    }
    else if ((ProtocolType == RIP) && isRIPstart) {
        updateRIP();
    }
    else {
        updateForwarding();
        increaseTimeInBuffer();
    }
}

void Router::updateForwarding() {
    QString data;
    for (port* p : ports) {
        if (!p->isBufferEmpty()) {
            data = p->receive();
            QStringList parts = data.split('#');
            if (parts[0] == EBGP) {
                updateDistanceVector(rip::decode(parts[1]), p->getPortNumber());
            }
            else if (parts[0] == MESSAGE) {
                forward(data);
            }
        }
    }
}

void Router::startEBGP() {
    iseBGP = true;
    QString data = rip::incode(distanceVector);
    for (port* p : ports) {
        if (p->destination->asID != asID) {
            send(data, p, EBGP);
        }
    }
}

void Router::turnOffProtocols() {
    this->isOSPFstart = false;
    this->isRIPstart = false;
    if (ProtocolType == OSPF) {
        distanceVector.table = lsdb.table;
    }
}

void Router::startIBGP() {
    if (iseBGP) {
        //printTable();
        qDebug() << "Router :" << _id << "iBGP finished.";
        return;
    }
    if (asID == 1) {
        int eBGP = 14;
        for (int i = 14; i <= 16; i++) {
            if (distanceVector.table[eBGP] > distanceVector.table[i]) {
                eBGP = i;
            }
        }
        for (int i = 17; i <= 24; i++) {
            forwardingTable[i] = forwardingTable[eBGP];
        }
        for (int i = 33; i <= 37; i++) {
            forwardingTable[i] = forwardingTable[eBGP];
        }
    }
    else {
        int eBGP = 17;
        for (int i = 17; i <= 19; i++) {
            if (distanceVector.table[eBGP] > distanceVector.table[i]) {
                eBGP = i;
            }
        }
        for (int i = 1; i <= 16; i++) {
            forwardingTable[i] = forwardingTable[eBGP];
        }
        for (int i = 25; i <= 32; i++) {
            forwardingTable[i] = forwardingTable[eBGP];
        }
    }
    //printTable();
    qDebug() << "Router :" << _id << "iBGP finished.";
}
