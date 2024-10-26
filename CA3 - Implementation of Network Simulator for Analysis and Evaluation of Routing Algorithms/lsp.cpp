#include "lsp.h"

lsp::lsp(QObject *parent)
    : QObject{parent}
{}

lsp* lsp::decode(const QString &line) {
    lsp *packet = new lsp();
    QStringList sections = line.split(" $ ");
    QStringList pairs = sections[0].split(", ");
    for (const QString &pair : pairs) {
        QStringList keyValue = pair.split(" ");
        if (keyValue.size() == 2) {
            int key = keyValue[0].toInt();
            int value = keyValue[1].toInt();
            packet->table.insert(key, value);
        }
    }
    QStringList visitedNodes = sections[1].split(" ");
    for (const QString &node : visitedNodes) {
        packet->visitedNodeIDs.append(node.toInt());
    }
    return packet;
}

QString lsp::incode(const lsp &packet) {
    QStringList pairs;
    for (auto it = packet.table.begin(); it != packet.table.end(); ++it) {
        pairs.append(QString::number(it.key()) + " " + QString::number(it.value()));
    }
    QStringList visitedNodes;
    for (int nodeID : packet.visitedNodeIDs) {
        visitedNodes.append(QString::number(nodeID));
    }
    return pairs.join(", ") + " $ " + visitedNodes.join(" ");
}
