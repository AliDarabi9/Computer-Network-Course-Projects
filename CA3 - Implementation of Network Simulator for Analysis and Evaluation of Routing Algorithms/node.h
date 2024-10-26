#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <port.h>
#include <QString>
#include <QDebug.h>
#include <QThread>
#include <packet.h>

const char OSPF = 'O';
const char RIP = 'R';
const char PROTOCOL = 'P';
const char EBGP = 'E';

class node : public QObject
{
    Q_OBJECT
public:
    explicit node(QObject *parent = nullptr);
    virtual void bindDestPort(int portNumber, port* destPort) = 0;
    virtual void addNewPort(int portNumber) = 0;
    virtual port* getPortByPortNumber(int portNumber) = 0;
    int getID();

public slots:
    virtual void update() = 0;
    virtual void startProtocol(uint idAS);
    virtual void printTable();
    virtual void startEBGP();
    virtual void turnOffProtocols();
    virtual void startIBGP();
    virtual void startPCs(int destID);

protected:
    inline static uint ID = 0;
    const uint _id;
    uint asID;
    QString IP;

signals:
    void OSPFFinished();
    void RIPFinished();
};

#endif // NODE_H
