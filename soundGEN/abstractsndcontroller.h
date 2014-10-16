#ifndef ABSTRACTSNDCONTROLLER_H
#define ABSTRACTSNDCONTROLLER_H

#include <QObject>
#include <string.h>
#include <stdio.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "base_functions.h"

typedef double (*PlaySoundFunction) (int,unsigned int,double);

typedef double (*GenSoundFunction) (double, double, double, PlaySoundFunction);

struct GenSoundChannelInfo {
    double freq;
    double k;
    double amp;
    double fr;
    double ar;
    QString function_text;
    GenSoundFunction channel_fct;
};

class AbstractSndController
{
public:
    virtual unsigned int getChannelsCount() = 0;
    virtual double getFrequency() const = 0;
    virtual double getInstFreq(unsigned int channel) = 0;
    virtual double getInstAmp(unsigned int channel) = 0;
    virtual GenSoundFunction getChannelFunction(unsigned int channel) = 0;
    virtual FMOD::System *getFmodSystem() = 0;
    virtual FMOD_CREATESOUNDEXINFO getFmodSoundCreateInfo() = 0;
    static void ERRCHECK(FMOD_RESULT op_result);
};

#endif // ABSTRACTSNDCONTROLLER_H
