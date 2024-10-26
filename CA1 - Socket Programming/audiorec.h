#ifndef AUDIOREC_H
#define AUDIOREC_H

#include <QAudioDevice>
#include <QAudioSource>
#include <QBuffer>
#include <QCoreApplication>
#include <QFile>
#include <QMediaRecorder>
#include <QObject>
#include <QTimer>

class audioRec : public QObject
{
    Q_OBJECT
public:
    explicit audioRec(QVector<QBuffer *> *buffer, QObject *parent = nullptr);
    QVector<QBuffer *> *destinationBuffer;
    QBuffer *buff;
    QAudioSource *audio;
    QAudioFormat format;
    int count = 0;
    void run();
    void handleStateChanged(QAudio::State newState);
    void stopRecording();
    void toggleMuteState();

private:
    bool isMute;
signals:
    void finished();
};

#endif // AUDIOREC_H
