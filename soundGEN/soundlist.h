#ifndef SOUNDLIST_H
#define SOUNDLIST_H

#include <limits>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <QString>
#include <QVector>
#include <QList>
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
    unsigned int tag;
};

class SoundList
{
private:
    QVector<GenSoundRecord*> baseSoundsList;
    AbstractSndController *sc;
    unsigned int curr_tag;
    void ConvertSoundBuffer(void *buf, int length, int bits_count, void **outbuf, unsigned int *outlength);
    void removeSound(int i, bool removeFromList = true);
    void clearSounds();
public:
    SoundList(AbstractSndController* base_controller);
    ~SoundList();
    void setSound(int index, QString new_file, QString new_function, unsigned int tag = 0);
    QString getFunctionsText();
    double playSound(int index, unsigned int channel, double t);
    void InitSounds();
    unsigned int getTag();
    void setTag(unsigned int newtag);
};

#endif // SOUNDLIST_H
