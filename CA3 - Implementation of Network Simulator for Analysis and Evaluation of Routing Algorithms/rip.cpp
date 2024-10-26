#include "rip.h"

rip::rip(QObject *parent)
    : QObject{parent}
{}

rip* rip::decode(const QString &line) {
    rip *packet = new rip();
    QStringList pairs = line.split(", ");
    for (const QString &pair : pairs) {
        QStringList keyValue = pair.split(" ");
        if (keyValue.size() == 2) {
            int key = keyValue[0].toInt();
            int value = keyValue[1].toInt();
            packet->table.insert(key, value);
        }
    }
    return packet;
}

QString rip::incode(const rip &packet) {
    QStringList pairs;
    for (auto it = packet.table.begin(); it != packet.table.end(); ++it) {
        pairs.append(QString::number(it.key()) + " " + QString::number(it.value()));
    }
    return pairs.join(", ");
}
