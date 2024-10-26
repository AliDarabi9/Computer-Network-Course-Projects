#include <QApplication>
#include <QBuffer>
#include <QMediaRecorder>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <QcoreApplication>
#include <audioplay.h>
#include <audiorec.h>
#include <boost/asio.hpp>
#include <receive.h>
#include <send.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextStream qin(stdin);
    QTextStream qout(stdout);

    QVector<QBuffer *> packetBuffIn;
    QVector<QBuffer *> packetBuffOut;

    QTimer *timer;

    qout << "Send port : " << Qt::endl;
    QString sendPort = qin.readLine();
    qout << "Receive port : " << Qt::endl;
    QString receivePort = qin.readLine();

    audioRec *rec;
    audioplay *play;

    class ::send *send;
    class ::receive *receiver;

    QString username;
    if (sendPort.toInt() < receivePort.toInt()) {
        username = "User 1";
        send = new class ::send(sendPort.toInt(), &packetBuffOut, "127.0.0.1");
        receiver = new receive(receivePort.toInt(), &packetBuffIn);
    } else {
        username = "User 2";
        receiver = new receive(receivePort.toInt(), &packetBuffIn);
        send = new class ::send(sendPort.toInt(), &packetBuffOut, "127.0.0.1");
    }
    qout << "You are " << username << Qt::endl;
    rec = new audioRec(&packetBuffOut);
    QObject::connect(rec, &audioRec::finished, rec, &audioRec::run);
    rec->run();

    play = new audioplay(&packetBuffIn);
    QObject::connect(play, &audioplay::finished, play, &audioplay::run);
    play->run();

    timer = new QTimer(send);
    QObject::connect(timer, &QTimer::timeout, send, &send::startSending);
    timer->start(50);

    QTimer::singleShot(100, receiver,&receive::Start);

    QWidget widget;
    QPushButton *mute = new QPushButton("Mute: " + username);
    QObject::connect(mute, &QPushButton::clicked, rec, &audioRec::toggleMuteState);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mute);

    widget.setLayout(mainLayout);
    widget.show();

    return a.exec();
}
