#include "audiorec.h"

audioRec::audioRec(QVector<QBuffer *> *buffer, QObject *parent)
    : QObject{parent}
{
    isMute = false;
    destinationBuffer = buffer;
    buff = nullptr;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::UInt8);
}

void audioRec::run()
{
    buff = new QBuffer();
    buff->open(QIODevice::WriteOnly);
    audio = new QAudioSource(format);
    connect(audio, &QAudioSource::stateChanged, this, &audioRec::handleStateChanged);

    QTimer::singleShot(2500, this, &audioRec::stopRecording);
    if (!isMute) {
        audio->start(buff);
    }
}

void audioRec::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::StoppedState:
        if (audio->error() != QAudio::NoError) {
            // Error handling
        }
        break;

    case QAudio::ActiveState:
        break;

    default:
        // ...
        break;
    }
}

void audioRec::stopRecording()
{
    if (!isMute) {
        audio->stop();
    }
    destinationBuffer->push_back(buff);
    delete audio;
    emit finished();
}

void audioRec::toggleMuteState()
{
    isMute = !isMute;
    qDebug() << "is mute : " << isMute;
}
