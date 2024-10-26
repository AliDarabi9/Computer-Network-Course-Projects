#ifndef PC_H
#define PC_H

#include <QObject>
#include <QThread>
#include "node.h"

class pc : public node
{
    Q_OBJECT
public:
    explicit pc(uint idAS, QObject *parent = nullptr);
    void bindDestPort(int portNumber, port* destPort);
    void addNewPort(int portNumber);
    port* getPortByPortNumber(int portNumber);

public slots:
    void startPCs(int destID);
    void update();

private:
    port* sourcePort;

signals:
};

#endif // PC_H
