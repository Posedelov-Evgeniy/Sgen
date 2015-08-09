#ifndef SNDCONTROLLER_H
#define SNDCONTROLLER_H

#include <QApplication>
#include <QTimer>
#include <QtGui>
#include <QtCore/QCoreApplication>
#include <QProcess>
#include <QVector>
#include <QMap>
#include <QCryptographicHash>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "base_functions.h"
#include "abstractsndcontroller.h"
#include "soundlist.h"
#include "classes/environmentinfo.h"
#include "classes/sndanalyzer.h"

#if defined(WIN32) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
    #define __PACKED                         /* dummy */
#else
    #define __PACKED __attribute__((packed)) /* gcc packed */
#endif

double base_play_sound(int i, unsigned int c, double t);
double get_variable_value(unsigned int);

enum SndControllerPlayMode { SndPlay, SndExport };

class SndController : public QObject, public AbstractSndController
{
    Q_OBJECT
private:
    static SndController* _self_controller;
    SndController(QObject *parent = 0);
    ~SndController();

    Q_DISABLE_COPY(SndController);

    QString getCurrentParseHash();
    bool checkHash(bool emptyCheck);
    bool parseFunctions();
    double getResult(unsigned int channel, double current_t);

    void resetParams();
    void play_cycle(FMOD::Sound *sound);
    void export_cycle(FMOD::Sound *sound);
    void writeWavHeader(FILE *file, FMOD::Sound *sound, int length);

    bool is_stopping, is_running;
    double t, t_real;
    qint64 t_real_ms_unixtime;
    bool all_functions_loaded;
    unsigned int channels_count;
    double frequency;
    int export_max_t;
    QString export_filename;

    QVector<GenSoundChannelInfo*> channels;
    QMap<QString, double> *variables;
    QMap<QString, QString> *expressions;
    QStringList *inner_variables;

    SoundList *baseSoundList;
    QString text_functions, sound_functions;
    QString oldParseHash;
    QLibrary lib;
    QEventLoop *loop;
    QTimer *timer;
    QThread *process_thread;

    FMOD::System *system;
    FMOD_CREATESOUNDEXINFO  createsoundexinfo_gen, createsoundexinfo_sound;
    FMOD_RESULT result;
    SndControllerPlayMode process_mode;
    SndAnalyzer *analyzer;
    bool sound_system_initialized;
    unsigned int system_buffer_ms_size;

    QMutex buffer_mutex;
    qint32 *double_buff;
    unsigned int double_buff_size;
    bool variable_changed;

public:
    static SndController* Instance();
    static bool DeleteInstance();

    void fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen, bool use_second_buffer, bool for_second_buffer);
    void fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen, bool use_second_buffer);
    void fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen);
    double playSound(int index, unsigned int channel, double t);

    void setChannelsCount(unsigned int count);
    unsigned int getChannelsCount();

    double getFrequency() const;
    void setFrequency(double value);

    void setFunctionsStr(QString new_f);
    void setFunctionStr(unsigned int channel, QString new_text);
    void setAmp(unsigned int channel, double new_amp);
    void setFreq(unsigned int channel, double new_freq);

    double getInstFreq(unsigned int channel);
    double getInstAmp(unsigned int channel);
    double getT();
    GenSoundFunction getChannelFunction(unsigned int channel);

    FMOD::System *getFmodSystem();
    FMOD_CREATESOUNDEXINFO getFmodSoundCreateInfo();
    SoundList *getBaseSoundList() const;
    bool running();
    void run();
    void stop();
    void run_export(int seconds, QString filename);
    void stop_export();

    QMap<QString, double>* getVariables();
    QMap<QString, QString>* getExpressions();

    unsigned int getSystemBufferMsSize() const;
    void setSystemBufferMsSize(unsigned int value);

    QStringList* getInnerVariables() const;
    void setVariableChanged(bool value);

signals:
    void starting();
    void started();
    void stopped();
    void cycle_start();
    void write_message(QString message);
    void finished();
    void export_finished();
    void export_status(int percent);
protected:
    void initSoundSystem();
private slots:
    void process_sound();
    void updateTimer();
};

#endif // SNDCONTROLLER_H
