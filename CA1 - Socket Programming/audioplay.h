#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H

#include <QAudioDevice>
#include <QAudioSink>
#include <QAudioSource>
#include <QBuffer>
#include <QCoreApplication>
#include <QFile>
#include <QMediaDevices>
#include <QMediaRecorder>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVector>

class audioplay : public QObject
{
    Q_OBJECT
public:
    explicit audioplay(QVector<QBuffer *> *buffer, QObject *parent = nullptr);
    QVector<QBuffer *> *sourceBuffer;
    QBuffer *buff;
    QAudioSink *audio;
    QAudioFormat format;
    void run();
    void handleStateChanged(QAudio::State newState);
    void stopAudioOutput();

signals:
    void finished();
};

#endif // AUDIOPLAY_H
