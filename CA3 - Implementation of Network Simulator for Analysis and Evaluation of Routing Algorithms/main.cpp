#include <QCoreApplication>
#include <QTimer>
#include <QThread>
#include <QVector>
#include <QPair>
#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>
#include "node.h"
#include "pc.h"
#include "router.h"
using bindInfo = QPair<int, node*>;

const int AS_ONE_ROUTERS_COUNT = 16;
const int AS_TWO_ROUTERS_COUNT = 8;
const int AS_ONE_PCS_COUNT = 8;
const int AS_TWO_PCS_COUNT = 5;

const int MAX_NUMBER_OF_PACKETS = 30;

void createThreadsForNodes(QVector<node*> &nodes, QVector<QThread*> &threads) {
    for (node* n : nodes) {
        QThread* thread = new QThread();
        n->moveToThread(thread);
        threads.push_back(thread);
        thread->start();
    }
}

void update(QVector<node*> nodes) {
    for (node* n : nodes) {
        QMetaObject::invokeMethod(n, "update", Qt::QueuedConnection);
    }
}

void bindNodes(QPair<int, node*> node1, QPair<int, node*> node2) {
    node1.second->addNewPort(node1.first);
    node2.second->addNewPort(node2.first);
    node1.second->bindDestPort(node1.first, node2.second->getPortByPortNumber(node2.first));
    node2.second->bindDestPort(node2.first, node1.second->getPortByPortNumber(node1.first));
}

QVector<std::tuple<int, int>> parseLine(const QString &line) {
    QVector<std::tuple<int, int>> result;
    QStringList entries = line.split(',', Qt::SkipEmptyParts);
    for (QString entry : entries) {
        entry = entry.trimmed();
        QStringList parts = entry.split(' ', Qt::SkipEmptyParts);
        int id = parts[0].toInt();
        int port = parts[1].toInt();
        result.append(std::make_tuple(id, port));
    }
    return result;
}

node* findByID(int id, QVector<node*> nodes) {
    for (node* n : nodes) {
        if (id == n->getID()) {
            return n;
        }
    }
    return nullptr;
}

void bindByInputLine(QVector<node*> nodes, QString line) {
    auto result = parseLine(line);
    auto dataNodeOne = result[0];
    auto dataNodeTwo = result[1];
    int nodeOneID = std::get<0>(dataNodeOne);
    int nodeOnePort = std::get<1>(dataNodeOne);
    int nodeTwoID = std::get<0>(dataNodeTwo);
    int nodeTwoPort = std::get<1>(dataNodeTwo);
    node* nodeOne = findByID(nodeOneID, nodes);
    node* nodeTwo = findByID(nodeTwoID, nodes);
    bindNodes(bindInfo(nodeOnePort, nodeOne), bindInfo(nodeTwoPort, nodeTwo));
}

void createTopology(QVector<node*> nodes) {
    QFile file("C:\\Users\\alida\\OneDrive\\Desktop\\CN-CA3\\CA3\\Topology.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file Topology.txt";
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        bindByInputLine(nodes, line);
    }
    file.close();
}

void initializeRouters(QVector<node*>& nodes, const char& protocol, int routerCount, uint asID) {
    for (int i = 0; i < routerCount; ++i) {
        Router* router = new Router(protocol, asID);
        nodes.push_back(router);
    }
}

void initializePCs(QVector<node*>& nodes, int PcCount, uint asID) {
    for (int i = 0; i < PcCount; ++i) {
        pc* p = new pc(asID);
        nodes.push_back(p);
    }
}

void setupApplicationCleanup(QCoreApplication& app, QVector<QThread*>& threads, QVector<node*>& nodes) {
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&threads, &nodes]() {
        for (QThread* thread : threads) {
            thread->quit();
        }
        for (QThread* thread : threads) {
            thread->wait();
            delete thread;
        }
        for (node* n : nodes) {
            delete n;
        }
    });
}

void ripRun(const QVector<node*>& nodes, uint asID) {
    for (node* n : nodes) {
        QMetaObject::invokeMethod(n, "startProtocol", Qt::QueuedConnection, Q_ARG(uint, asID));
    }
}

void setupRipRun(const QVector<node*>& nodes, uint asID) {
    QTimer::singleShot(2, [nodes, asID]() {
        ripRun(nodes, asID);
    });
}

void setupRepeatingTimer(int interval, const QVector<node*>& nodes) {
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [nodes]() {
        update(nodes);
    });
    timer->start(interval);
}

void runOSPFOnNodesSequentially(bool* finishFlag, const QVector<node*>& nodes, int routerCount, uint asID, int startIndex, int index) {
    if (index >= routerCount + startIndex) {
        QMetaObject::invokeMethod(nodes[startIndex], "printTable", Qt::QueuedConnection);
        *finishFlag = true;
        return;
    }
    node* currentNode = nodes[index];
    QMetaObject::invokeMethod(currentNode, "startProtocol", Qt::QueuedConnection, Q_ARG(uint, asID));
    QObject::connect(currentNode, &node::OSPFFinished, [&, finishFlag, routerCount, asID, startIndex, index, nodes]() {
        runOSPFOnNodesSequentially(finishFlag, nodes, routerCount, asID, startIndex, index + 1);
    });
}

void finishInASProtocols(const QVector<node*>& nodes) {
    for (node* n : nodes) {
        QMetaObject::invokeMethod(n, "turnOffProtocols", Qt::QueuedConnection);
    }
}

void sendEBGP(const QVector<node*>& nodes, QVector<int>& edgeRoutersIDs) {
    for (int index : edgeRoutersIDs) {
        QMetaObject::invokeMethod(nodes[index], "startEBGP", Qt::QueuedConnection);
    }
}

QPair<int, int> selectPCtoSend() {
    QRandomGenerator randomGenerator = QRandomGenerator::securelySeeded();
    int firstPC = randomGenerator.bounded(24, 32);
    int secondPC = randomGenerator.bounded(32, 37);
    if (randomGenerator.bounded(0, 2) == 1) {
        return QPair<int, int>(firstPC, secondPC);
    } else {
        return QPair<int, int>(secondPC, firstPC);
    }
}

void printRouterTables(const QVector<node*>& nodes) {
    QTextStream input(stdin);
    QString routerID;
    while (!input.atEnd()) {
        qDebug() << "Enter router ID (1 to 16 for AS ONE and 17 to 24 for AS TWO):";
        routerID = input.readLine().trimmed().toUpper();
        if (routerID.isEmpty()) {
            break;
        }
        bool ok;
        int id = routerID.toInt(&ok);

        if (ok && id > 0 && id <= 24) {
            QMetaObject::invokeMethod(nodes[id-1], "printTable", Qt::QueuedConnection);
        } else {
            qDebug() << "Invalid router ID. Please enter a valid number between 1 and 24";
        }
    }
}

QVector<int> generatePoissonNumbers(int count = MAX_NUMBER_OF_PACKETS, int mean = MAX_NUMBER_OF_PACKETS / 2) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<> d(mean);
    QVector<int> numbers;
    numbers.reserve(count);
    for (int i = 0; i < count; ++i) {
        int num = d(gen);
        while (num < 1 || num > MAX_NUMBER_OF_PACKETS) {
            num = d(gen);
        }
        numbers.push_back(num);
    }
    return numbers;
}

QVector<int> countOccurrences() {
    QVector<int> input = generatePoissonNumbers();
    QVector<int> counts(MAX_NUMBER_OF_PACKETS, 0);
    for (int num : input) {
        if (num >= 1 && num <= MAX_NUMBER_OF_PACKETS) {
            counts[num - 1]++;
        }
    }
    return counts;
}

void sendFromPC(const QVector<node*>& nodes) {
    QVector<int> occurrenceCount = countOccurrences();
    for (int cnt : occurrenceCount) {
        for (int i = 0; i < cnt; i++) {
            QPair<int, int> ids = selectPCtoSend();
            QMetaObject::invokeMethod(nodes[ids.first], "startPCs", Qt::QueuedConnection, Q_ARG(int, (ids.second + 1)));
        }
    }
    QTimer::singleShot(2000, [nodes]() {
        printRouterTables(nodes);
    });
}

void sendIBGP(const QVector<node*>& nodes) {
    for (node* n : nodes) {
        QMetaObject::invokeMethod(n, "startIBGP", Qt::QueuedConnection);
    }
    QTimer::singleShot(1000, [nodes]() {
        sendFromPC(nodes);
    });
}

void finishProtocolTimer(QVector<int>& edgeRoutersIDs ,const QVector<node*>& nodes, bool* ASOneProtocolFinished, bool* ASTwoProtocolFinished) {
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [timer, &edgeRoutersIDs, nodes, ASOneProtocolFinished, ASTwoProtocolFinished]() {
        if (*ASOneProtocolFinished && *ASTwoProtocolFinished) {
            finishInASProtocols(nodes);
            sendEBGP(nodes, edgeRoutersIDs);
            QTimer::singleShot(10, [nodes]() {
                qDebug() << "START IBGP.";
                sendIBGP(nodes);
            });
            timer->stop();
            timer->deleteLater();
        }
    });
    timer->start(100);
}

QPair<char, char> getInputProtocolTypes() {
    QTextStream input(stdin);
    QString asOneProtocolStr, asTwoProtocolStr;

    qDebug() << "Enter protocol type for AS One (RIP/OSPF):";
    asOneProtocolStr = input.readLine().trimmed().toUpper();

    qDebug() << "Enter protocol type for AS Two (RIP/OSPF):";
    asTwoProtocolStr = input.readLine().trimmed().toUpper();

    char ASOneProtocol, ASTwoProtocol;

    if (asOneProtocolStr == "RIP") {
        ASOneProtocol = RIP;
    } else if (asOneProtocolStr == "OSPF") {
        ASOneProtocol = OSPF;
    } else {
        qCritical() << "Invalid protocol type for AS One!";
        exit(1);
    }
    if (asTwoProtocolStr == "RIP") {
        ASTwoProtocol = RIP;
    } else if (asTwoProtocolStr == "OSPF") {
        ASTwoProtocol = OSPF;
    } else {
        qCritical() << "Invalid protocol type for AS Two!";
        exit(1);
    }
    return qMakePair(ASOneProtocol, ASTwoProtocol);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QVector<QThread*> threads;
    QVector<int> edgeRoutersIDs = {13, 14, 15, 16, 17, 18};
    QVector<node*> nodes;

    QPair<char, char> protocols = getInputProtocolTypes();
    char ASOneProtocol = protocols.first;
    char ASTwoProtocol = protocols.second;

    bool* ASOneProtocolFinished = new bool();
    bool* ASTwoProtocolFinished = new bool();
    initializeRouters(nodes, ASOneProtocol, AS_ONE_ROUTERS_COUNT, 1);
    initializeRouters(nodes, ASTwoProtocol, AS_TWO_ROUTERS_COUNT, 2);
    initializePCs(nodes, AS_ONE_PCS_COUNT, 3);
    initializePCs(nodes, AS_TWO_PCS_COUNT, 3);
    createTopology(nodes);
    createThreadsForNodes(nodes, threads);
    setupApplicationCleanup(a, threads, nodes);
    setupRepeatingTimer(1, nodes);

    if (ASOneProtocol == RIP) {
        setupRipRun(nodes, 1);
        QObject::connect(nodes[0], &node::RIPFinished, [&nodes, ASOneProtocolFinished]() {
            qDebug() << "RIP Finished.";
            QMetaObject::invokeMethod(nodes[0], "printTable", Qt::QueuedConnection);
            *ASOneProtocolFinished = true;
        });
    } else {
        runOSPFOnNodesSequentially(ASOneProtocolFinished, nodes, AS_ONE_ROUTERS_COUNT, 1, 0, 0);
    }
    if (ASTwoProtocol == RIP) {
        setupRipRun(nodes, 2);
        QObject::connect(nodes[16], &node::RIPFinished, [&nodes, ASTwoProtocolFinished]() {
            qDebug() << "RIP Finished.";
            QMetaObject::invokeMethod(nodes[16], "printTable", Qt::QueuedConnection);
            *ASTwoProtocolFinished = true;
        });
    } else {
        runOSPFOnNodesSequentially(ASTwoProtocolFinished, nodes, AS_TWO_ROUTERS_COUNT, 2, 16, 16);
    }

    finishProtocolTimer(edgeRoutersIDs, nodes, ASOneProtocolFinished, ASTwoProtocolFinished);

    return a.exec();
}
