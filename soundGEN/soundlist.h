#ifndef SOUNDLIST_H
#define SOUNDLIST_H

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <QString>
#include <QVector>
#include <QDebug>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "abstractsndcontroller.h"

struct GenSoundRecord {
    QString sound_file;
    QString sound_function;
    FMOD::Sound *base_sound;
    unsigned int soundLenPcmBytes;
    unsigned int soundLen;
    unsigned int channels_count;
    double frequency;
    qint32 *pcmData;
};

class SoundList
{
private:
    QVector<GenSoundRecord*> baseSoundsList;
    AbstractSndController *sc;
    void ConvertSoundBuffer(void *buf, int length, int bits_count, void **outbuf, unsigned int *outlength);
public:
    SoundList(AbstractSndController* base_controller);
    void clearSounds();
    void AddSound(QString new_file, QString new_function);
    QString getFunctionsText();
    double playSound(int index, unsigned int channel, double t);
    void InitSounds();
};

#endif // SOUNDLIST_H
