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
    baseSoundList = new SoundList();
    is_running = false;
}

SndController::~SndController()
{
    mfct.left_channel_fct = 0;
    mfct.right_channel_fct = 0;
    baseSoundList->clearSounds();
    delete baseSoundList;
}

void SndController::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
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

void SndController::fillBuffer(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    signed short *stereo16bitbuffer = (signed short *)data;

    datalen = datalen>>2;  // >>2 = 16bit stereo (4 bytes per sample)

    if (mfct.left_channel_fct && mfct.right_channel_fct) {

        double l_pprev=0, r_pprev=0, l_prev=0, r_prev=0, l_curr=0, r_curr=0, l_cnt=0, r_cnt=0, l_a = 0, r_a = 0;

        for (count=0; count<datalen; count++)
        {
            l_curr = getLResult();
            r_curr = getRResult();
            *(stereo16bitbuffer++) = (signed short)(l_curr * 32767.0);   // left channel
            *(stereo16bitbuffer++) = (signed short)(r_curr * 32767.0);   // right channel
            t += 1.0/44100.0;

            l_curr = fabs(l_curr);
            r_curr = fabs(r_curr);

            if (l_pprev>0 || r_pprev>0) {
                if (l_prev>l_curr && l_prev>l_pprev) l_cnt++;
                if (r_prev>r_curr && r_prev>r_pprev) r_cnt++;
            }

            if (l_a<l_curr) l_a = l_curr;
            if (r_a<r_curr) r_a = r_curr;

            l_pprev = l_prev;
            r_pprev = r_prev;
            l_prev = l_curr;
            r_prev = r_curr;
        }

        l_fr = 0.5 * l_cnt/(datalen/44100.0);
        r_fr = 0.5 * r_cnt/(datalen/44100.0);

        l_ar = l_a<=1.0 ? l_a : 1.0;
        r_ar = r_a<=1.0 ? r_a : 1.0;
    }
}

bool SndController::parseFunctions()
{
    QCoreApplication *app = QCoreApplication::instance();
    QString error = "";
    bool add_base_functions = false;

    if (lib.isLoaded()) {
        lib.unload();
    }

    QDir dir(app->applicationDirPath());
    dir.mkdir("efr");

    if (QFile::exists(app->applicationDirPath()+"/base_functions.h")) {
        if (QFile::exists(app->applicationDirPath()+"/efr/base_functions.cpp"))
        {
            QFile::remove(app->applicationDirPath()+"/efr/base_functions.cpp");
        }
        QFile::copy(app->applicationDirPath()+"/base_functions.cpp", app->applicationDirPath()+"/efr/base_functions.cpp");
        if (QFile::exists(app->applicationDirPath()+"/efr/base_functions.h"))
        {
            QFile::remove(app->applicationDirPath()+"/efr/base_functions.h");
        }
        QFile::copy(app->applicationDirPath()+"/base_functions.h", app->applicationDirPath()+"/efr/base_functions.h");
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

    QFile file(app->applicationDirPath()+"/efr/main.h");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&file);
    out << "#include <math.h>\n";
    out << "#include <string.h>\n";
    out << "#include <stdio.h>\n";
    if (add_base_functions) out << "#include \"base_functions.h\"\n";
    out << "typedef double (*PlaySoundFunction) (int,unsigned int,double);\n";
    out << spec_func_pref << " double sound_func_l(double t, double k, double f, PlaySoundFunction __bFunction);\n";
    out << spec_func_pref << " double sound_func_r(double t, double k, double f, PlaySoundFunction __bFunction);\n";
    file.close();

    QFile file2(app->applicationDirPath()+"/efr/"+main_file_name);
    file2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out2(&file2);
    out2 << "#include \"main.h\"\n";
    out2 << spec_namespace;
    out2 << "\n" + text_functions + "\n";
    out2 << "PlaySoundFunction BaseSoundFunction;\n";
    out2 << baseSoundList->getFunctionsText() + "\n";
    out2 << spec_func_pref << "double sound_func_l(double t, double k, double f, PlaySoundFunction __bFunction) { BaseSoundFunction=__bFunction; return (double) ("+text_l+"); };\n";
    out2 << spec_func_pref << "double sound_func_r(double t, double k, double f, PlaySoundFunction __bFunction) { BaseSoundFunction=__bFunction; return (double) ("+text_r+"); };\n";
    out2 << "int main() {return 0;};\n";
    file2.close();

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString tcmd;

        QStringList env_list(QProcess::systemEnvironment());
        int idx = env_list.indexOf(QRegExp("^VS110COMNTOOLS=.*", Qt::CaseInsensitive));
        if (idx > -1)
        {
            QStringList windir = env_list[idx].split('=');
            QDir rdir(windir[1]);
            rdir.cdUp();
            rdir.cdUp();
            rdir.cd("VC");
            pConsoleProc->setWorkingDirectory(rdir.absolutePath());
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
        if (QFile::exists(app->applicationDirPath()+"/efr/main.so"))
        {
            QFile::remove(app->applicationDirPath()+"/efr/main.so");
        }

        QFile file3(app->applicationDirPath()+"/efr/Makefile");
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

        QString tcmd = "make -C \""+app->applicationDirPath()+"/efr\" -f Makefile";
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
        lib.setFileName(app->applicationDirPath()+"/efr/main");
        lib.load();
        mfct.left_channel_fct = (GenSoundFunction)(lib.resolve("sound_func_l"));
        mfct.right_channel_fct = (GenSoundFunction)(lib.resolve("sound_func_r"));
    } else {
        mfct.left_channel_fct = 0;
        mfct.right_channel_fct = 0;
    }

    return mfct.left_channel_fct && mfct.right_channel_fct;
}

void SndController::resetParams() {
    text_l = "sin(k*t)";
    text_r = "cos(k*t)";
    freq_l = freq_r = 500;
    kL = kR = freq_l*2.0*M_PI;
    amp_l = amp_r = 1.0;
    l_fr = r_fr = 0;
    l_ar = r_ar = 0;
    t = 0.0;
    baseSoundList->clearSounds();
}

double SndController::getLResult()
{
    return amp_l * mfct.left_channel_fct(t, kL, freq_l, base_play_sound);
}

double SndController::getRResult()
{
    return amp_r * mfct.right_channel_fct(t, kR, freq_r, base_play_sound);
}

int SndController::doprocess() {
    FMOD::System           *system;
    FMOD::Sound            *sound;
    FMOD::Channel          *channel = 0;
    FMOD_RESULT             result;
    FMOD_MODE               mode = FMOD_2D | FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_SOFTWARE;
    int                     channels = 2;
    FMOD_CREATESOUNDEXINFO  createsoundexinfo;
    unsigned int            version;
    QTextStream             console(stdout);

    is_stopping = false;
    resetParams();

    emit starting();

    emit write_message(tr("Initialization..."));

    console << tr("Starting with:") << " " << endl;
    console << tr("Function L:") << " " << text_l << endl;
    console << tr("Function R:") << " " << text_r << endl;
    console << tr("Amp L:") << " " << amp_l << endl;
    console << tr("Amp R:") << " " << amp_r << endl;
    console << tr("Freq L:") << " " << freq_l << endl;
    console << tr("Freq R:") << " " << freq_r << endl;

    if (!parseFunctions()) {
        emit write_message(tr("Error in functions!"));
        return 0;
    }

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
        return 0;
    }

    emit started();

    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    mode |= FMOD_CREATESTREAM;

    memset(&createsoundexinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    createsoundexinfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);              /* required. */
    createsoundexinfo.decodebuffersize  = 44100;                                       /* Chunk size of stream update in samples.  This will be the amount of data passed to the user callback. */
    createsoundexinfo.length            = 44100 * channels * sizeof(signed short) * 2; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo.numchannels       = channels;                                    /* Number of channels in the sound. */
    createsoundexinfo.defaultfrequency  = 44100;                                       /* Default playback rate of sound. */
    createsoundexinfo.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
    createsoundexinfo.pcmreadcallback   = pcmreadcallback;                             /* User callback for reading. */
    createsoundexinfo.pcmsetposcallback = pcmsetposcallback;                           /* User callback for seeking. */

    result = system->createSound(0, mode, &createsoundexinfo, &sound);
    ERRCHECK(result);

    baseSoundList->setSystem(system);
    baseSoundList->InitSounds();

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

            printf("Time %02d:%02d:%02d/%02d:%02d:%02d : %s\n", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
            fflush(stdout);
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
    baseSoundList->clearSounds();
    result = system->close();
    ERRCHECK(result);
    result = system->release();
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

void SndController::SetLFunctionStr(QString new_text_l) {
    text_l = new_text_l;
}

void SndController::SetRFunctionStr(QString new_text_r) {
    text_r = new_text_r;
}

void SndController::SetFunctionsStr(QString new_f) {
    text_functions = new_f;
}

void SndController::SetLAmp(double new_amp_l) {
    amp_l = new_amp_l;
}

void SndController::SetRAmp(double new_amp_r) {
    amp_r = new_amp_r;
}

void SndController::SetLFreq(double new_freq_l) {
    freq_l = new_freq_l;
    kL = freq_l*2.0*M_PI;
}

void SndController::SetRFreq(double new_freq_r) {
    freq_r = new_freq_r;
    kR = freq_r*2.0*M_PI;
}

double SndController::getInstLFreq() {
    return l_fr;
}
double SndController::getInstRFreq() {
    return r_fr;
}
double SndController::getInstLAmp() {
    return l_ar;
}
double SndController::getInstRAmp() {
    return r_ar;
}

GenSoundFunction SndController::getLeftFunction()
{
    return mfct.left_channel_fct;
}

GenSoundFunction SndController::getRightFunction()
{
    return mfct.right_channel_fct;
}

void SndController::AddSound(QString new_file, QString new_function)
{
    baseSoundList->AddSound(new_file, new_function);
}

bool SndController::running()
{
    return is_running;
}
