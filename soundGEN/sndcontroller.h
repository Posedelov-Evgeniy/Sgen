#ifndef SNDCONTROLLER_H
#define SNDCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QTimer>
#include "fmod.hpp"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <QtGui>
#include <QtCore/QCoreApplication>
#include <QProcess>

typedef double (*PlaySoundFunction) (double);

typedef double (*GenSoundFunction) (double, double, double, PlaySoundFunction);

struct GenSoundFunctions {
    GenSoundFunction left_channel_fct;
    GenSoundFunction right_channel_fct;
};


class SndController : public QObject
{
    Q_OBJECT
private:
    static SndController* _self_controller;
    SndController(QObject *parent = 0);
    Q_DISABLE_COPY(SndController);

    bool parseFunctions();
    double getLResult();
    double getRResult();

    int doprocess();
    void resetParams();
    bool is_stopped;

    GenSoundFunctions mfct;
    double freq_l, freq_r, kL, kR;
    double amp_l, amp_r;
    double t;
    double l_fr, r_fr;
    double l_ar, r_ar;
    FMOD::Sound *base_sound;
    unsigned int soundLenPcmBytes;
    signed short *pcmData;

    QString sound_file;
    QString text_l;
    QString text_r;
    QString text_functions;
    QLibrary lib;
    QEventLoop loop;
    QEventLoop loopStop;
public:
    static SndController* Instance();
    static bool DeleteInstance();

    void fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen);
    double playSound(int i, double t);

    void SetSoundFile(QString new_file);
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
