#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <lsp.h>

class port : public QObject
{
    Q_OBJECT
public:
    explicit port(int number, int nodeID, uint idAS, QObject *parent = nullptr);
    void send(QString data);
    QString receive();
    void setPortDest(port* destPort);
    bool isPortNumber(int number);
    bool isBufferEmpty();
    int getNodeID();
    int getPortNumber();
    port* destination;
    uint asID;
    QVector<QString> buffer;

private:
    int portNumber;
    int _nodeID;

signals:
};

#endif // PORT_H
