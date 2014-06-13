#include "sndcontroller.h"

static mainFct mfct;
static double freq_l, freq_r;
static double amp_l, amp_r;
static double t;

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    signed short *stereo16bitbuffer = (signed short *)data;

    datalen = datalen>>2;  // >>2 = 16bit stereo (4 bytes per sample)

    if (mfct.left_channel_fct && mfct.right_channel_fct) {
        double kL = freq_l*2.0*M_PI;
        double kR = freq_r*2.0*M_PI;
        for (count=0; count<datalen; count++)
        {
            *stereo16bitbuffer++ = (signed short)(amp_l * mfct.left_channel_fct(t, kL, freq_l) * 32767.0d);    // left channel
            *stereo16bitbuffer++ = (signed short)(amp_r * mfct.right_channel_fct(t, kR, freq_r) * 32767.0d);    // right channel
            t += 1.0/44100.0d;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    /*
        This is useful if the user calls Sound::setPosition and you want to seek your data accordingly.
    */
    return FMOD_OK;
}

SndController::SndController(QObject *parent) :
    QObject(parent)
{
}

void SndController::resetParams() {
    text_l = "sin(k*t)";
    text_r = "cos(k*t)";
    freq_l = freq_r = 500;
    amp_l = amp_r = 1.0;
    t = 0.0;
}

bool SndController::parseFunctions(QCoreApplication *app) {

    if (lib.isLoaded()) {
        lib.unload();
    }

    QDir dir(app->applicationDirPath());
    dir.mkdir("efr");

    QFile file(app->applicationDirPath()+"/efr/main.h");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&file);
    out << "extern \"C\" double sound_func_l(double t, double k, double f);\r\n";
    out << "extern \"C\" double sound_func_r(double t, double k, double f);\r\n";
    file.close();

    QFile file2(app->applicationDirPath()+"/efr/main.c");
    file2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out2(&file2);
    out2 << "#include <math.h>\r\n";
    out2 << "#include <string.h>\r\n";
    out2 << "#include <stdio.h>\r\n";
    out2 << "#include \"main.h\"\r\n";
    out2 << text_functions + "\r\n";
    out2 << "extern \"C\" double sound_func_l(double t, double k, double f) { return (double) ("+text_l+"); };\r\n";
    out2 << "extern \"C\" double sound_func_r(double t, double k, double f) { return (double) ("+text_r+"); };\r\n";
    out2 << "int main() {return 0;};\r\n";
    file2.close();

    QFile file3(app->applicationDirPath()+"/efr/Makefile");
    file3.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out3(&file3);
    out3 << "x64: main.c\r\n";
    out3 << "	g++ -m64 -Wall -fPIC -c main.c\r\n";
    out3 << "	g++ -shared -o main.so main.o\r\n";
    out3 << "clean:\r\n";
    out3 << "	rm -f main.o\r\n";
    out3 << "	rm -f main.so\r\n";
    file3.close();

    QProcess* pConsoleProc = new QProcess;
    QString tcmd = "make -C \""+app->applicationDirPath()+"/efr\" -f Makefile";
    pConsoleProc->execute(tcmd);
    pConsoleProc->close();
    delete pConsoleProc;

    lib.setFileName(app->applicationDirPath()+"/efr/main");
    lib.load();
    mfct.left_channel_fct = (Fct)(lib.resolve("sound_func_l"));
    mfct.right_channel_fct = (Fct)(lib.resolve("sound_func_r"));

    return mfct.left_channel_fct && mfct.right_channel_fct;
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

    is_stopped = false;
    resetParams();

    emit starting();

    printf("Starting with: \r");
    printf("Function L: %s\r", qPrintable(text_l));
    printf("Function R: %s\r", qPrintable(text_r));
    printf("Amp L: %f\r", amp_l);
    printf("Amp R: %f\r", amp_r);
    printf("Freq L: %f\r", freq_l);
    printf("Freq R: %f\r", freq_r);
    fflush(stdout);

    if (!parseFunctions(QCoreApplication::instance())) {
        printf("Error in functions!\r");
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
    createsoundexinfo.length            = 44100 * channels * sizeof(signed short) * 5; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
    createsoundexinfo.numchannels       = channels;                                    /* Number of channels in the sound. */
    createsoundexinfo.defaultfrequency  = 44100;                                       /* Default playback rate of sound. */
    createsoundexinfo.format            = FMOD_SOUND_FORMAT_PCM16;                     /* Data format of sound. */
    createsoundexinfo.pcmreadcallback   = pcmreadcallback;                             /* User callback for reading. */
    createsoundexinfo.pcmsetposcallback = pcmsetposcallback;                           /* User callback for seeking. */

    result = system->createSound(0, mode, &createsoundexinfo, &sound);
    ERRCHECK(result);

    /*
        Play the sound.
    */
    result = system->playSound(FMOD_CHANNEL_FREE, sound, 0, &channel);
    ERRCHECK(result);

    /*
        Main loop.
    */
    do
    {
        system->update();

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

            printf("Time %02d:%02d:%02d/%02d:%02d:%02d : %s\r", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
            fflush(stdout);
        }

        //Sleep(20);
        QTimer::singleShot(5000, &loop, SLOT(quit()));
        loop.exec();

    } while (!is_stopped);

    printf("\n");

    if (channel) {
        channel->setPaused(true);
    }

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    is_stopped = false;

    return 0;
}

void SndController::run() {
    doprocess();
    emit stopped();
}

void SndController::stop() {
    is_stopped = true;
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
}

void SndController::SetRFreq(double new_freq_r) {
    freq_r = new_freq_r;
}
