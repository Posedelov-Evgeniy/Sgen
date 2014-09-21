#include "sndcontroller.h"

SndController *SndController::_self_controller = 0;


FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    SndController::Instance()->fillBuffer(sound, data, datalen);
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        This is useful if the user calls Sound::setPosition and you want to seek your data accordingly.
    */
    return FMOD_OK;
}

double base_play_sound(int i, unsigned int c, double t) {
    return SndController::Instance()->playSound(i, c, t);
}

SndController::SndController(QObject *parent) :
    QObject(parent)
{
    baseSoundList = new SoundList(this);
    is_running = false;
    all_functions_loaded = false;
    channels_count = 0;
    frequency = 0;
    oldParseHash = "";
    sound_functions = "";

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

    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    memset(&createsoundexinfo_sound, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    createsoundexinfo_sound.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO); /* required. */
    createsoundexinfo_sound.format            = FMOD_SOUND_FORMAT_PCM32;        /* Data format of sound. */
    createsoundexinfo_gen = createsoundexinfo_sound;
    createsoundexinfo_gen.pcmreadcallback   = pcmreadcallback;                  /* User callback for reading. */
    createsoundexinfo_gen.pcmsetposcallback = pcmsetposcallback;                /* User callback for seeking. */

    setChannelsCount(2);
    setFrequency(44100.0);
}

SndController::~SndController()
{
    setChannelsCount(0);

    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    delete baseSoundList;
}

SndController *SndController::Instance()
{
    if(!_self_controller)
    {
        _self_controller = new SndController();
    }
    return _self_controller;
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
    if (count<channels.size()) {
        while (count>=0 && count<channels.size()) {
            delete channels.last();
            channels.remove(channels.size()-1);
        }
    } else if (count>channels.size()) {
        while (count>channels.size()) {
            GenSoundChannelInfo *info = new GenSoundChannelInfo();
            info->amp = 1;
            info->freq = 500;
            info->channel_fct = 0;
            info->function_text = "sin(k*t)";
            info->k = info->freq*2.0*M_PI;
            info->fr = 0;
            info->ar = 0;
            channels.append(info);
        }
    }

    channels_count = count;

    createsoundexinfo_sound.length            = ((unsigned int) frequency) * channels_count * sizeof(qint32) * 2; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo_sound.numchannels       = channels_count;                                                   /* Number of channels in the sound. */
    createsoundexinfo_gen.length = createsoundexinfo_sound.length;
    createsoundexinfo_gen.numchannels = createsoundexinfo_sound.numchannels;
}

unsigned int SndController::getChannelsCount()
{
    return channels_count;
}

void SndController::fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    qint32 *buffer = (qint32*)data;
    qint32 max_val = std::numeric_limits<qint32>::max();

    datalen = datalen/(channels_count*sizeof(qint32));

    if (all_functions_loaded)
    {
        for(unsigned int i=0; i<channels_count; i++)
        {
            double curr = 0;
            double pprev = 0;
            double prev = 0;
            double cnt = 0;
            double amp = 0;

            for (count=0; count<datalen; count++)
            {
                curr = getResult(i, t+count/frequency);
                buffer[count*channels_count + i] = (qint32)(curr * max_val);
                curr = fabs(curr);

                if (pprev>0 && prev>curr && prev>pprev) {
                    cnt++;
                }

                if (amp<curr) amp = curr;

                pprev = prev;
                prev = curr;
            }
            channels.at(i)->fr = 0.5 * cnt/(datalen/frequency);
            channels.at(i)->ar = amp<=1.0 ? amp : 1.0;
        }

        t += datalen/frequency;
    }
}

QString SndController::getCurrentParseHash()
{
    QCoreApplication *app = QCoreApplication::instance();
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(EnvironmentInfo::getConfigsPath().toLatin1());
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        hash.addData(QString::number(QFile::exists(app->applicationDirPath()+"/efr/main.dll")).toLatin1());
    #else
        hash.addData(QString::number(QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/main.so")).toLatin1());
    #endif
    hash.addData(QString::number(channels_count).toLatin1());
    hash.addData(QString::number(lib.isLoaded()).toLatin1());
    hash.addData(sound_functions.toLatin1());
    hash.addData(text_functions.toLatin1());
    for(int i=0;i<channels_count;i++) {
        hash.addData(channels.at(i)->function_text.toLatin1());
    }

    return hash.result().toHex();
}

bool SndController::checkHash(bool emptyCheck)
{
    QString parseHash = "";

    if (emptyCheck || !oldParseHash.isEmpty()) {
        parseHash = getCurrentParseHash();
        if (!emptyCheck || oldParseHash.isEmpty()) {
            qDebug() << tr("Hash is: ") << parseHash;
        }
    }

    if (!emptyCheck && !oldParseHash.isEmpty() && !parseHash.isEmpty() && oldParseHash==parseHash) {
        qDebug() << tr("Hashes equals - no need to parse functions");
        return true;
    }

    if (!parseHash.isEmpty()) {
        oldParseHash = parseHash;
    }

    return false;
}

bool SndController::parseFunctions()
{
    if (checkHash(false)) return true;

    QCoreApplication *app = QCoreApplication::instance();
    QString error = "";
    bool add_base_functions = false;
    all_functions_loaded = false;
    unsigned int i;

    if (lib.isLoaded()) {
        lib.unload();
    }

    QDir dir(EnvironmentInfo::getConfigsPath());
    dir.mkdir("efr");

    if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/base_functions.h")) {
        if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp"))
        {
            QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp");
        }
        QFile::copy(EnvironmentInfo::getConfigsPath()+"/base_functions.cpp", EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp");
        if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h"))
        {
            QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h");
        }
        QFile::copy(EnvironmentInfo::getConfigsPath()+"/base_functions.h", EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h");
        add_base_functions = true;
    }

    QProcess* pConsoleProc = new QProcess;

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString main_file_name = "main.cpp";
        QString spec_func_pref = "extern \"C\" __declspec(dllexport)";
        QString spec_namespace = "using namespace std;\n";
    #else
        QString main_file_name = "main.c";
        QString spec_func_pref = "extern \"C\"";
        QString spec_namespace = "";
    #endif

    QFile file(EnvironmentInfo::getConfigsPath()+"/efr/main.h");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&file);
    out << "#include <math.h>\n";
    out << "#include <string.h>\n";
    out << "#include <stdio.h>\n";
    if (add_base_functions) out << "#include \"base_functions.h\"\n";
    out << "typedef double (*PlaySoundFunction) (int,unsigned int,double);\n";
    for(i=0;i<channels_count;i++) {
        out << spec_func_pref << " double sound_func_"+QString::number(i)+"(double t, double k, double f, PlaySoundFunction __bFunction);\n";
    }
    file.close();

    QFile file2(EnvironmentInfo::getConfigsPath()+"/efr/"+main_file_name);
    file2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out2(&file2);
    out2 << "#include \"main.h\"\n";
    out2 << spec_namespace;
    out2 << "\nPlaySoundFunction BaseSoundFunction;\n";
    out2 << "\n" + sound_functions + "\n";
    out2 << "\n" + text_functions + "\n";
    for(i=0;i<channels_count;i++) {
        out2 << spec_func_pref << "double sound_func_"+QString::number(i)+"(double t, double k, double f, PlaySoundFunction __bFunction) { BaseSoundFunction=__bFunction; return (double) ("+channels.at(i)->function_text+"); };\n";
    }
    out2 << "int main() {return 0;};\n";
    file2.close();

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString tcmd;
        QString vcdir = EnvironmentInfo::getVCPath();
        if (!vcdir.isEmpty())
        {
            pConsoleProc->setWorkingDirectory(vcdir);
            pConsoleProc->start("vcvarsall.bat x86");
            pConsoleProc->waitForFinished();
            qDebug() << pConsoleProc->workingDirectory() << endl;
        }

        pConsoleProc->setWorkingDirectory(app->applicationDirPath()+"/efr");
        qDebug() << pConsoleProc->workingDirectory() << endl;

        if (QFile::exists(app->applicationDirPath()+"/efr/main.dll"))
        {
            QFile::remove(app->applicationDirPath()+"/efr/main.dll");
        }

        if (add_base_functions) {
            pConsoleProc->start("cl.exe /c /EHsc base_functions.cpp");
            pConsoleProc->waitForFinished();
            qDebug() <<  pConsoleProc->readAll() << endl;
            pConsoleProc->start("lib base_functions.obj");
            pConsoleProc->waitForFinished();
            qDebug() <<  pConsoleProc->readAll() << endl;
            tcmd = "cl.exe /LD main.cpp /DLL /link base_functions.lib";
        } else {
            tcmd = "cl.exe /LD main.cpp /link /DLL";
        }
    #else
        if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/main.so"))
        {
            QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/main.so");
        }

        QFile file3(EnvironmentInfo::getConfigsPath()+"/efr/Makefile");
        file3.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream out3(&file3);

        out3 << "CC = g++\n";
        out3 << "MODULES= main.o base_functions.o\n";
        out3 << "OBJECTS=\n";
        out3 << "RCLOBJECTS= main.c main.h base_functions.cpp base_functions.h\n";
        out3 << "all: main.so\n";
        out3 << "main.so:$(MODULES)\n";
        out3 << "	$(CC) -shared $(MODULES) -o main.so\n";
        out3 << "base_functions.o: $(RCLOBJECTS)\n";
        out3 << "	g++ -m64 -Wall -fPIC -c base_functions.cpp -o base_functions.o\n";
        out3 << "main.o: $(RCLOBJECTS)\n";
        out3 << "	g++ -m64 -Wall -fPIC -c main.c -o main.o\n";
        out3 << "clean:\n";
        out3 << "	rm -f *.o\n";
        out3 << "	rm -f main.so\n";
        file3.close();

        QString tcmd = "make -C \""+EnvironmentInfo::getConfigsPath()+"/efr\" -f Makefile";
    #endif
    qDebug() <<  tcmd << endl;

    pConsoleProc->start(tcmd, QProcess::ReadOnly);
    if(pConsoleProc->waitForFinished()==true)
    {
       QByteArray b = pConsoleProc->readAllStandardError();
       #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
           qDebug() <<  pConsoleProc->readAll() << endl;
           error = "";
       #else
           error = QString(b);
       #endif
       qDebug() <<  error << endl;
    }
    pConsoleProc->close();
    delete pConsoleProc;

    if (error.isEmpty()) {
        lib.setFileName(EnvironmentInfo::getConfigsPath()+"/efr/main");
        lib.load();
        all_functions_loaded = true;
        for(i=0;i<channels_count;i++) {
            channels.at(i)->channel_fct = (GenSoundFunction)(lib.resolve(qPrintable("sound_func_"+QString::number(i))));
            all_functions_loaded = all_functions_loaded && channels.at(i)->channel_fct;
        }
        checkHash(true);
    }

    if (!all_functions_loaded) {
        oldParseHash = "";
        for(i=0;i<channels_count;i++) {
            channels.at(i)->channel_fct = 0;
        }
    }

    return all_functions_loaded;
}

double SndController::getResult(unsigned int channel, double current_t)
{
    GenSoundChannelInfo *info = channels.at(channel);
    return info->amp * info->channel_fct(current_t, info->k, info->freq, base_play_sound);
}

void SndController::resetParams()
{
    for(unsigned int i=0; i<channels_count; i++) {
        GenSoundChannelInfo *info = channels.at(i);
        info->amp = 1;
        info->freq = 500;
        info->k = info->freq*2.0*M_PI;
        info->ar = 0;
        info->fr = 0;
        info->function_text = "sin(k*t)";
    }
    t = 0.0;
}

SoundList *SndController::getBaseSoundList() const
{
    return baseSoundList;
}

double SndController::getFrequency() const
{
    return frequency;
}

void SndController::setFrequency(double value)
{
    frequency = value;
    createsoundexinfo_sound.decodebuffersize  = (unsigned int) frequency;                                         /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    createsoundexinfo_sound.length            = ((unsigned int) frequency) * channels_count * sizeof(qint32) * 2; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo_sound.defaultfrequency  = (unsigned int) frequency;                                         /* Default playback rate of sound. */
    createsoundexinfo_gen.decodebuffersize = createsoundexinfo_sound.decodebuffersize;
    createsoundexinfo_gen.length = createsoundexinfo_sound.length;
    createsoundexinfo_gen.defaultfrequency = createsoundexinfo_sound.defaultfrequency;
}

int SndController::doprocess()
{
    FMOD::Sound            *sound;
    FMOD::Channel          *channel = 0;
    FMOD_MODE               mode = FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE;
    QTextStream             console(stdout);
    GenSoundChannelInfo    *info;

    is_stopping = false;
    resetParams();

    emit starting();

    emit write_message(tr("Initialization..."));

    console << tr("Starting with:") << " " << endl;
    for(unsigned int i=0; i<channels_count; i++) {
        info = channels.at(i);
        console << tr("Function %num%:").replace("%num%",QString::number(i)) << " " << info->function_text << endl;
        console << tr("Amp %num%:").replace("%num%",QString::number(i)) << " " << info->amp << endl;
        console << tr("Freq %num%:").replace("%num%",QString::number(i)) << " " << info->freq << endl;
    }

    sound_functions = baseSoundList->getFunctionsText();
    console << tr("Sounds:") << "[" << sound_functions << "]" << endl;

    if (!parseFunctions()) {
        emit write_message(tr("Error in functions!"));
        return 0;
    }

    emit started();

    mode |= FMOD_CREATESTREAM;

    result = system->createSound(0, mode, &createsoundexinfo_gen, &sound);
    ERRCHECK(result);

    /*
        Play the sound.
    */
    result = system->playSound(FMOD_CHANNEL_FREE, sound, 0, &channel);
    ERRCHECK(result);

    emit write_message(tr("Playing"));

    is_running = true;
    /*
        Main loop.
    */
    do
    {
        system->update();

        emit cycle_start();
        emit write_message(tr("Time: %sec%s").replace("%sec%",QString::number(t, 'f', 1)));

        if (channel)
        {
            unsigned int ms;
            unsigned int lenms;
            bool         playing;
            bool         paused;

            channel->isPlaying(&playing);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPaused(&paused);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }
        }

        //Sleep(20);
        QTimer::singleShot(2000, &loop, SLOT(quit()));
        loop.exec();

    } while (!is_stopping);

    printf("\n");

    if (channel) {
        channel->setPaused(true);
    }

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);

    is_stopping = false;
    is_running = false;

    emit write_message(tr("Stopped"));

    return 0;
}

void SndController::run() {
    doprocess();
    emit stopped();
}

void SndController::stop() {
    is_stopping = true;
    loop.exit();
}

void SndController::setFunctionsStr(QString new_f) {
    text_functions = new_f;
}

void SndController::setFunctionStr(unsigned int channel, QString new_text)
{
    channels.at(channel)->function_text = new_text;
}

void SndController::setAmp(unsigned int channel, double new_amp)
{
    channels.at(channel)->amp = new_amp;
}

void SndController::setFreq(unsigned int channel, double new_freq)
{
    channels.at(channel)->freq = new_freq;
    channels.at(channel)->k = new_freq*2.0*M_PI;
}

double SndController::getInstFreq(unsigned int channel)
{
    return channels.at(channel)->fr;
}

double SndController::getInstAmp(unsigned int channel)
{
    return channels.at(channel)->ar;
}

GenSoundFunction SndController::getChannelFunction(unsigned int channel)
{
    if (lib.isLoaded() && channel>=0 && channel<channels.size())
        return channels.at(channel)->channel_fct;
    else
        return 0;
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
