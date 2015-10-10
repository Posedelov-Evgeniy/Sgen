#ifndef ABSTRACTSNDCONTROLLER_H
#define ABSTRACTSNDCONTROLLER_H

#include <QObject>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "signalcontroller.h"
#include "base_functions.h"

class AbstractSndController: public SignalController
{
    Q_OBJECT
public:
    AbstractSndController(QObject *parent = 0);

    virtual FMOD::System *getFmodSystem() = 0;
    virtual FMOD_CREATESOUNDEXINFO getFmodSoundCreateInfo() = 0;
    static void ERRCHECK(FMOD_RESULT op_result);
};

#endif // ABSTRACTSNDCONTROLLER_H
