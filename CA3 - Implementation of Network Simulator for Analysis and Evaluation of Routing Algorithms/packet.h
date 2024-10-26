#ifndef PACKET_H
#define PACKET_H

#include <QDebug>
#include <QObject>

const char MESSAGE = 'M';

class packet : public QObject
{
    Q_OBJECT
public:
    explicit packet(QString inMessage, int sourceId, int destId, QObject *parent = nullptr);
    static packet* decode(const QString &line);
    static QString incode(const packet &inPacket);
    void addRouterToPath(int id);
    QString printPacket();
    QString message;
    int sourceID;
    QString sourceIP;
    int destID;
    QString destIP;
    QVector<QString> routersInPath;
    int cycleInBuffer;
    void increaseBufferTime();
signals:
};

#endif // PACKET_H
