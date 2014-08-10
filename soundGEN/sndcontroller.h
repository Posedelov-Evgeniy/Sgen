#ifndef SNDCONTROLLER_H
#define SNDCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QTimer>
#include <string.h>
#include <stdio.h>
#include <QtGui>
#include <QtCore/QCoreApplication>
#include <QProcess>
#include <QVector>
#include <math.h>
#include "base_functions.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "soundlist.h"

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

double base_play_sound(int i, unsigned int c, double t);

class SndController : public QObject
{
    Q_OBJECT
private:
    static SndController* _self_controller;
    SndController(QObject *parent = 0);
    ~SndController();

    Q_DISABLE_COPY(SndController);

    bool parseFunctions();
    double getResult(unsigned int channel, double current_t);

    int doprocess();
    void resetParams();
    bool is_stopping, is_running;
    double t;
    bool all_functions_loaded;
    unsigned int channels_count;

    QVector<GenSoundChannelInfo*> channels;

    SoundList *baseSoundList;
    QString text_functions;
    QLibrary lib;
    QEventLoop loop;

    FMOD::System *system;
    FMOD_RESULT result;
    void ERRCHECK(FMOD_RESULT op_result);
public:
    static SndController* Instance();
    static bool DeleteInstance();

    void fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen);
    double playSound(int index, unsigned int channel, double t);

    void setChannelsCount(unsigned int count);
    unsigned int getChannelsCount();

    void setFunctionsStr(QString new_f);
    void setFunctionStr(unsigned int channel, QString new_text);
    void setAmp(unsigned int channel, double new_amp);
    void setFreq(unsigned int channel, double new_freq);

    double getInstFreq(unsigned int channel);
    double getInstAmp(unsigned int channel);
    GenSoundFunction getChannelFunction(unsigned int channel);

    FMOD::System *getFmodSystem();
    void addSound(QString new_file, QString new_function);
    bool running();
signals:
    void starting();
    void started();
    void stopped();
    void cycle_start();
    void write_message(QString message);
public slots:
    void run();
    void stop();
};

#endif // SNDCONTROLLER_H
