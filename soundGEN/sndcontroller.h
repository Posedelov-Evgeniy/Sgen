#ifndef SNDCONTROLLER_H
#define SNDCONTROLLER_H

#include <QApplication>
#include <QTimer>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "base_functions.h"
#include "abstractsndcontroller.h"
#include "soundlist.h"
#include "classes/sndanalyzer.h"

#if defined(WIN32) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
    #define __PACKED                         /* dummy */
#else
    #define __PACKED __attribute__((packed)) /* gcc packed */
#endif

double base_play_sound(int i, unsigned int c, double t);
double get_variable_value(unsigned int);

enum SndControllerPlayMode { SndPlay, SndExport };

class SndController : public AbstractSndController
{
    Q_OBJECT
private:
    static SndController* _self_controller;
    SndController(QObject *parent = 0);
    ~SndController();

    Q_DISABLE_COPY(SndController);

    void play_cycle(FMOD::Sound *sound);
    void export_cycle(FMOD::Sound *sound);
    void writeWavHeader(FILE *file, FMOD::Sound *sound, int length);

    bool is_stopping, is_running;

    qint64 t_real_ms_unixtime;
    int export_max_t;
    QString export_filename;

    SoundList *baseSoundList;
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
    bool analize_is_active;

public:
    static SndController* Instance();
    static bool DeleteInstance();

    double playSound(int index, unsigned int channel, double t);

    virtual void setChannelsCount(unsigned int count);

    virtual void setFrequency(double value);
    double getT();

    FMOD::System *getFmodSystem();
    FMOD_CREATESOUNDEXINFO getFmodSoundCreateInfo();
    SoundList *getBaseSoundList() const;
    bool running();
    void run();
    void stop();
    void run_export(int seconds, QString filename);
    void stop_export();

    unsigned int getSystemBufferMsSize() const;
    void setSystemBufferMsSize(unsigned int value);

    void setPlayAnalizeActivity(bool active);
    bool getPlayAnalizeActivity() const;
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
