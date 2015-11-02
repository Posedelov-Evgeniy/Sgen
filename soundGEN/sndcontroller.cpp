#include "sndcontroller.h"

SndController *SndController::_self_controller = 0;


FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    SndController::Instance()->fillBuffer(data, datalen);
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        This is useful if the user calls Sound::setPosition and you want to seek your data accordingly.
    */
    return FMOD_OK;
}

double get_variable_value(unsigned int varid) {
    CVariables *variables = SndController::Instance()->getVariables();
    return variables->value(variables->keys().at(varid));
}

double base_play_sound(int i, unsigned int c, double t) {
    return SndController::Instance()->playSound(i, c, t);
}

SndController::SndController(QObject *parent) :
    AbstractSndController(parent)
{
    analize_is_active = true;
    sound_system_initialized = false;
    baseSoundList = new SoundList(this);
    loop = new QEventLoop();
    timer = new QTimer();

    analyzer = new SndAnalyzer();
    analyzer->setTop_harmonic(1);

    is_stopping = false;
    is_running = false;
    process_mode = SndPlay;
    system_buffer_ms_size = 1000;

    setBasePlaySoundFunction(base_play_sound);
    setVariableValueFunction(get_variable_value);

    unsigned int            version;
    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        exit(-1);
    }

    process_thread = new QThread();
    QObject::connect(process_thread,SIGNAL(started()),this,SLOT(process_sound()));
    QObject::connect(this,SIGNAL(finished()),process_thread,SLOT(terminate()));
    moveToThread(process_thread);
    loop->moveToThread(process_thread);
    timer->moveToThread(process_thread);
    #if !defined(ANDROID) && !defined(__ANDROID__)
    timer->setTimerType(Qt::PreciseTimer);
    #endif
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    memset(&createsoundexinfo_sound, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    createsoundexinfo_sound.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO); /* required. */
    createsoundexinfo_sound.format            = FMOD_SOUND_FORMAT_PCM32;        /* Data format of sound. */
    createsoundexinfo_gen = createsoundexinfo_sound;
    createsoundexinfo_gen.pcmreadcallback   = pcmreadcallback;                  /* User callback for reading. */
    createsoundexinfo_gen.pcmsetposcallback = pcmsetposcallback;                /* User callback for seeking. */

    setChannelsCount(2);
    setFrequency(44100.0);
    resetParams();

    initSoundSystem();
}

SndController::~SndController()
{
    setChannelsCount(0);

    process_thread->quit();

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    delete loop;
    delete timer;
    delete baseSoundList;
    delete analyzer;


    delete process_thread;
}

SndController *SndController::Instance()
{
    if(!_self_controller)
    {
        _self_controller = new SndController(0);
    }
    return _self_controller;
}

void SndController::initSoundSystem()
{
    if (sound_system_initialized) {
        sound_system_initialized = false;
        result = system->close();
        ERRCHECK(result);
    }

    FMOD_SPEAKERMODE spmode;
    int set_channels = 32;

    switch(channels_count) {
        case 8:
            spmode = FMOD_SPEAKERMODE_7POINT1; break;
        case 6:
            spmode = FMOD_SPEAKERMODE_5POINT1; break;
        case 5:
            spmode = FMOD_SPEAKERMODE_SURROUND; break;
        case 4:
            spmode = FMOD_SPEAKERMODE_QUAD; break;
        case 2:
            spmode = FMOD_SPEAKERMODE_STEREO; break;
        case 1:
            spmode = FMOD_SPEAKERMODE_MONO; break;
        default:
            spmode = FMOD_SPEAKERMODE_DEFAULT;
    }

    result = system->setSoftwareFormat(createsoundexinfo_sound.defaultfrequency, spmode, 0);
    if (result != FMOD_OK && spmode != FMOD_SPEAKERMODE_DEFAULT) {
        result = system->setSoftwareFormat(createsoundexinfo_sound.defaultfrequency, FMOD_SPEAKERMODE_DEFAULT, 0);
    } else if (result == FMOD_OK && spmode != FMOD_SPEAKERMODE_DEFAULT) {
        set_channels = channels_count;
    }

    result = system->init(set_channels, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);
    sound_system_initialized = true;
}

bool SndController::DeleteInstance()
{
    if(_self_controller)
    {
        delete _self_controller;
        _self_controller = 0;
        return true;
    }
    return false;
}

double SndController::playSound(int index, unsigned int channel, double t)
{
    return baseSoundList->playSound(index, channel, t);
}

void SndController::setChannelsCount(unsigned int count)
{
    SignalController::setChannelsCount(count);
    createsoundexinfo_sound.length            = ((unsigned int) frequency) * channels_count * sizeof(qint32); /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo_sound.numchannels       = channels_count;                                               /* Number of channels in the sound. */
    createsoundexinfo_gen.length = createsoundexinfo_sound.length;
    createsoundexinfo_gen.numchannels = createsoundexinfo_sound.numchannels;
    if (sound_system_initialized) initSoundSystem();
}

SoundList *SndController::getBaseSoundList() const
{
    return baseSoundList;
}

unsigned int SndController::getSystemBufferMsSize() const
{
    return system_buffer_ms_size;
}

void SndController::setSystemBufferMsSize(unsigned int value)
{
    system_buffer_ms_size = value;
    createsoundexinfo_sound.decodebuffersize  = (unsigned int) round(frequency * system_buffer_ms_size / 1000); /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    createsoundexinfo_sound.length            = ((unsigned int) frequency) * channels_count * sizeof(qint32);   /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo_gen.decodebuffersize = createsoundexinfo_sound.decodebuffersize;
    createsoundexinfo_gen.length = createsoundexinfo_sound.length;
}

void SndController::setPlayAnalizeActivity(bool active)
{
    analize_is_active = active;
}

bool SndController::getPlayAnalizeActivity() const
{
    return analize_is_active;
}

void SndController::setFrequency(double value)
{
    SignalController::setFrequency(value);
    createsoundexinfo_sound.decodebuffersize  = (unsigned int) round(frequency * system_buffer_ms_size / 1000); /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    createsoundexinfo_sound.length            = ((unsigned int) frequency) * channels_count * sizeof(qint32);   /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo_sound.defaultfrequency  = (unsigned int) frequency;                                       /* Default playback rate of sound. */
    createsoundexinfo_gen.decodebuffersize = createsoundexinfo_sound.decodebuffersize;
    createsoundexinfo_gen.length = createsoundexinfo_sound.length;
    createsoundexinfo_gen.defaultfrequency = createsoundexinfo_sound.defaultfrequency;
    if (sound_system_initialized) initSoundSystem();
}

void SndController::writeWavHeader(FILE *file, FMOD::Sound *sound, int length)
{
    int             channels, bits;
    float           rate;

    if (!sound)
    {
        return;
    }

    fseek(file, 0, SEEK_SET);

    sound->getFormat  (0, 0, &channels, &bits);
    sound->getDefaults(&rate, 0);

    {
        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack(push, 1)
        #endif

        /*
            WAV Structures
        */
        typedef struct
        {
            signed char id[4];
            int 		size;
        } RiffChunk;

        struct
        {
            RiffChunk       chunk           __PACKED;
            unsigned short	wFormatTag      __PACKED;    /* format type  */
            unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
            unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
            unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
            unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
            unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
        } __PACKED FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

        struct
        {
            RiffChunk   chunk;
        } DataChunk = { {{'d','a','t','a'}, length } };

        struct
        {
            RiffChunk   chunk;
            signed char rifftype[4];
        } WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + length }, {'W','A','V','E'} };

        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack(pop)
        #endif

        /*
            Write out the WAV header.
        */
        fwrite(&WavHeader, sizeof(WavHeader), 1, file);
        fwrite(&FmtChunk, sizeof(FmtChunk), 1, file);
        fwrite(&DataChunk, sizeof(DataChunk), 1, file);
    }
}

void SndController::export_cycle(FMOD::Sound *sound)
{
    emit export_status(0);

    FILE *mainfile = fopen(qPrintable(export_filename), "wb");

    if (mainfile) {
        unsigned int datalength = 0;
        writeWavHeader(mainfile, sound, datalength);

        unsigned int sec_buff_size = ((unsigned int) frequency) * channels_count * sizeof(qint32);
        quint8 *buf = new quint8[sec_buff_size];
        resetT();
        for(int second = 0; second<export_max_t; second++) {
            fillBuffer(buf, sec_buff_size);
            datalength += fwrite(buf, 1, sec_buff_size, mainfile);
            emit export_status(round(100.0*second/export_max_t));
        }
        delete buf;

        if (datalength) {
            writeWavHeader(mainfile, sound, datalength);
        }

        fclose(mainfile);
        qDebug() << tr("Sound object written to file");

        emit export_status(100);
    } else {
        emit write_message(tr("Error write to file: %filename%").replace("%filename%",export_filename));
    }
}

void SndController::play_cycle(FMOD::Sound *sound)
{
    FMOD::Channel          *channel = 0;
    unsigned int i;

    /*
        Play the sound.
    */
    result = system->playSound(sound, 0, 0, &channel);
    ERRCHECK(result);

    variable_changed = false;
    buffer_mutex.lock();
    buffer_mutex.unlock();

    timer->start(system_buffer_ms_size >= 100 ? system_buffer_ms_size / 2 : 50);
    is_running = true;
    /*
        Main loop.
    */
    do
    {
        system->update();

        emit cycle_start();

        if (analize_is_active) {
            for(i=0; i<channels.size(); i++) {
                analyzer->function_fft_top_only(getChannelFunction(i), t - system_buffer_ms_size/2000.0, t + system_buffer_ms_size/2000.0, channels.at(i)->freq, 1*frequency);
                channels.at(i)->fr = analyzer->getInstFrequency();
                channels.at(i)->ar = channels.at(i)->amp * analyzer->getInstAmp();
            }
        }

        QTimer::singleShot(system_buffer_ms_size, loop, SLOT(quit()));
        loop->exec();
    } while (!is_stopping);
    timer->stop();

    if (channel) {
        channel->setPaused(true);
    }

    QMutexLocker locker(&buffer_mutex);
}

void SndController::process_sound()
{
    FMOD::Sound            *sound;
    FMOD_MODE               mode = FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL;
    QTextStream             console(stdout);
    GenChannelInfo    *info;
    bool parsed;

    resetT();
    is_stopping = false;
    emit write_message(tr("Initialization..."));

    int driver = -1;
    char soundcard[256];
    system->getDriver(&driver);
    if (driver >= 0) {
        system->getDriverInfo(driver, soundcard, sizeof(soundcard), 0, 0, 0, 0);
        console << tr("Driver:") << " " << QString(soundcard) << endl;
    }

    console << tr("Starting with:") << " " << endl;
    for(unsigned int i=0; i<channels_count; i++) {
        info = channels.at(i);
        console << tr("Function %num%:").replace("%num%",QString::number(i)) << " " << info->function_text << endl;
        console << tr("Amp %num%:").replace("%num%",QString::number(i)) << " " << info->amp << endl;
        console << tr("Freq %num%:").replace("%num%",QString::number(i)) << " " << info->freq << endl;
    }

    setSignalFunctions(baseSoundList->getFunctionsText());
    console << tr("Sounds:") << "[" << getSignalFunctions() << "]" << endl;

    parsed = parseFunctions();

    if (!parsed) {
        emit write_message(tr("Error in functions!"));
        process_thread->quit();
        return;
    }

    variableUpdated();

    if (process_mode == SndPlay) emit started();

    mode |= FMOD_CREATESTREAM;

    result = system->createSound(0, mode, &createsoundexinfo_gen, &sound);
    ERRCHECK(result);

    emit write_message(tr("Playing"));

    switch (process_mode) {
        case SndPlay:
            play_cycle(sound);
        break;
        case SndExport:
            export_cycle(sound);
        break;
    }

    printf("\n");

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);

    is_stopping = false;
    is_running = false;

    emit write_message(tr("Stopped"));
    if (process_mode == SndExport) emit export_finished();
    emit finished();
    if (process_mode == SndExport) {
        emit stopped();
        process_thread->quit();
    }

    return;
}

void SndController::updateTimer()
{
    t_real_ms_unixtime = QDateTime::currentMSecsSinceEpoch();
    t_real += timer->interval() / 1000.0;
    emit write_message(tr("Time: %sec%s").replace("%sec%",QString::number(t_real, 'f', 1)));
}

void SndController::run() {
    process_mode = SndPlay;
    process_thread->start();
    while (process_thread->isFinished()) {}
}

void SndController::stop() {
    is_stopping = true;
    loop->exit();
    process_thread->quit();
    while (!process_thread->isFinished()) {};
    emit stopped();
}

void SndController::run_export(int seconds, QString filename) {
    process_mode = SndExport;
    export_max_t = seconds;
    export_filename = filename;
    process_thread->start();
    while (process_thread->isFinished()) {}
}

double SndController::getT()
{
    double rt;
    rt = QDateTime::currentMSecsSinceEpoch() - t_real_ms_unixtime;
    if (!timer->isActive() || rt>2*timer->interval()) {
        rt = 0;
    } else {
        rt = rt / 1000;
    }
    return t_real + rt;
}

FMOD::System *SndController::getFmodSystem()
{
    return system;
}

FMOD_CREATESOUNDEXINFO SndController::getFmodSoundCreateInfo()
{
    return createsoundexinfo_sound;
}

bool SndController::running()
{
    return is_running;
}
