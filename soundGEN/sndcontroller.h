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
#include <math.h>
#include "base_functions.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#include "soundlist.h"

typedef double (*PlaySoundFunction) (int,unsigned int,double);

typedef double (*GenSoundFunction) (double, double, double, PlaySoundFunction);

struct GenSoundFunctions {
    GenSoundFunction left_channel_fct;
    GenSoundFunction right_channel_fct;
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
    double getLResult();
    double getRResult();

    int doprocess();
    void resetParams();
    bool is_stopping, is_running;

    double freq_l, freq_r, kL, kR;
    double amp_l, amp_r;
    double t;
    double l_fr, r_fr;
    double l_ar, r_ar;

    GenSoundFunctions mfct;
    SoundList *baseSoundList;
    QString text_l;
    QString text_r;
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

    void SetLFunctionStr(QString new_text_l);
    void SetRFunctionStr(QString new_text_r);
    void SetFunctionsStr(QString new_f);
    void SetLAmp(double new_amp_l);
    void SetRAmp(double new_amp_r);
    void SetLFreq(double new_freq_l);
    void SetRFreq(double new_freq_r);
    double getInstLFreq();
    double getInstRFreq();
    double getInstLAmp();
    double getInstRAmp();
    GenSoundFunction getLeftFunction();
    GenSoundFunction getRightFunction();
    FMOD::System *getFmodSystem();
    void AddSound(QString new_file, QString new_function);
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
