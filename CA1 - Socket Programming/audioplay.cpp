#include "audioplay.h"

audioplay::audioplay(QVector<QBuffer *> *buffer, QObject *parent)
    : QObject{parent}
{
    sourceBuffer = buffer;
    buff = nullptr;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::UInt8);
}

void audioplay::run()
{
    if (sourceBuffer->isEmpty() || buff != nullptr) {
        QTimer::singleShot(10, this, &audioplay::finished);
        return;
    }
    buff = sourceBuffer->first();
    sourceBuffer->pop_front();
    buff->open(QIODevice::ReadOnly);
    QAudioDevice info(QMediaDevices::defaultAudioOutput());

    audio = new QAudioSink(format, this);
    connect(audio, &QAudioSink::stateChanged, this, &audioplay::handleStateChanged);
    audio->start(buff);
}

void audioplay::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        audioplay::stopAudioOutput();
        emit finished();
        break;

    case QAudio::StoppedState:
        if (audio->error() != QAudio::NoError) {
            // Error handling
        }
        break;
    default:
        // ...
        break;
    }
}

void audioplay::stopAudioOutput()
{
    audio->stop();
    buff->close();

    delete audio;
    delete buff;
    buff = nullptr;
    emit finished();
}
