#ifndef SNDCONTROLLER_H
#define SNDCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QTimer>
#include "fmod.hpp"
#include "fmod_errors.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <QtGui>
#include <QtCore/QCoreApplication>
#include <QProcess>

typedef double (*Fct) (double, double, double);

struct channelInfo {
    Fct ch_function;
    double freq;
    double amp;
};

struct mainFct {
    Fct left_channel_fct;
    Fct right_channel_fct;
};

class SndController : public QObject
{
    Q_OBJECT
private:
    bool parseFunctions(QCoreApplication *app);
    int doprocess();
    void resetParams();
    bool is_stopped;
    QString text_l;
    QString text_r;
    QString text_functions;
    QLibrary lib;
    QEventLoop loop;
    QEventLoop loopStop;
public:
    explicit SndController(QObject *parent = 0);
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
public slots:
    void run();
    void stop();
};

#endif // SNDCONTROLLER_H
