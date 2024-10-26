#include "node.h"

node::node(QObject *parent)
    : QObject{parent}, _id(++node::ID)
{}

int node::getID() {
    return _id;
}

void node::startProtocol(uint idAS) {}

void node::printTable() {
    qDebug() << "THIS NODE DOES NOT HAVE A TABLE.";
}

void node::startEBGP() {}

void node::turnOffProtocols() {}

void node::startIBGP() {}

void node::startPCs(int destID) {}
