#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <node.h>
#include <lsp.h>
#include <rip.h>

class Router : public node
{
    Q_OBJECT

public:
    explicit Router(char protocol, uint idAS, QObject *parent = nullptr);
    void addNewPort(int portNumber);
    void bindDestPort(int portNumber, port* destPort);
    port* getPortByPortNumber(int portNumber);

public slots:
    void update();
    void startProtocol(uint idAS);
    void printTable();
    void startEBGP();
    void turnOffProtocols();
    void startIBGP();

private:
    QVector<port*> ports;
    QHash<int, int> forwardingTable;
    void send(QString data, port* p, char messageType);
    void forward(QString data);
    lsp lsdb;
    rip distanceVector;
    void updateLsp(lsp* newData, int portNumber);
    void sendLsp();
    void lspBackTrack();
    bool isStartOSPF = false;
    int lspIndex = 0;
    port* lspReceivePort;
    bool sendOSPF = false;
    bool isNodeVisited(int nodeID);
    void startOSPF();
    void updateOSPF();
    char ProtocolType;
    void updateRIP();
    bool updateDistanceVector(rip* newData, int portNumber);
    bool ripFinished = false;
    void sendRip();
    void startRIP();
    int ripConvergenceCondition = 0;
    bool ripFinishSignalEmited = false;
    bool isRIPstart = true;
    bool isOSPFstart = true;
    void handleOSPFFinish();
    void updateForwarding();
    bool iseBGP = false;
    void increaseTimeInBuffer();
};

#endif
