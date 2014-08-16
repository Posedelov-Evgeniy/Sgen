#include "soundlist.h"

SoundList::SoundList()
{
}

void SoundList::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

void SoundList::clearSounds()
{
    GenSoundRecord *rec;
    FMOD_RESULT result;
    foreach(rec, baseSoundsList)
    {
        if (rec->base_sound) {
            result = rec->base_sound->release();
            ERRCHECK(result);
        }
        if (rec->pcmData) {
            delete rec->pcmData;
        }
        delete rec;
    }
    baseSoundsList.clear();
}

void SoundList::setSystem(FMOD::System *new_system)
{
    system = new_system;
}

void SoundList::AddSound(QString new_file, QString new_function)
{
    GenSoundRecord *rec = new GenSoundRecord;

    rec->base_sound = 0;
    rec->pcmData = 0;
    rec->soundLenPcmBytes = 0;
    rec->sound_function = new_function;
    rec->sound_file = new_file;

    baseSoundsList.append(rec);
}

void SoundList::ConvertSoundBuffer(void *buf, int length, int bits_count, int channels_count, float frequency, void **outbuf, unsigned int *outlength)
{
    *outlength = 0;
    if (bits_count>32 || bits_count<8) return;
    if (frequency<10000 || frequency>200000) return;
    if (channels_count<1 || channels_count>20) return;
    if (length<=0) return;
    if (!buf) return;

    int bytes_count = bits_count >> 3;
    int buf_elements = length/bytes_count;
    float seconds_len = buf_elements/(channels_count*frequency);

    int out_channels = 2;
    int out_bits_count = 16;
    int out_bytes_count = out_bits_count >> 3;
    float out_frequency = 44100;
    unsigned int outbuf_length = out_bytes_count*out_channels*((unsigned int) out_frequency*seconds_len);

    void *mainbuff;
    qint32 *maxbuff = new qint32[buf_elements];
    unsigned int i, j, k;

    /* Формируем массив maxbuff с 32-битным представлением данных */
    if (bytes_count==4) {
        memcpy(maxbuff,buf,length);
    } else {
        qint32 interm_check = 1 << (bits_count - 1);
        qint32 interm_mask = -(1 << bits_count);
        double max_cval = interm_check;
        double max32bitval = std::numeric_limits<qint32>::max();

        for(i=0;i<buf_elements;i++) {
            qint32 intermediate = 0;
            for(j=0;j<bytes_count;j++) {
                intermediate |= ((quint8*) buf)[i*bytes_count+j] << (j << 3);
            }
            if (intermediate & interm_check) {
                intermediate |= interm_mask;
            }
            maxbuff[i] = (qint32) (intermediate/max_cval)*max32bitval;
        }
    }
    /* maxbuff сформирован */






    if (bytes_count==4) {
        mainbuff = maxbuff;
    } else {
        mainbuff = new qint8[outbuf_length];




        delete maxbuff;
    }


    *outbuf = mainbuff;
    *outlength = outbuf_length;
}

void SoundList::InitSounds()
{
    if (!system) return;

    FMOD_RESULT result;
    unsigned int read = 0;
    GenSoundRecord *rec;
    foreach(rec, baseSoundsList)
    {
        if (!rec->sound_file.isEmpty() && !rec->sound_function.isEmpty())
        {
            if (!rec->base_sound)
            {
                result = system->createSound(qPrintable(rec->sound_file), FMOD_OPENONLY | FMOD_ACCURATETIME, 0, &(rec->base_sound));
                ERRCHECK(result);
                result = rec->base_sound->setMode(FMOD_LOOP_OFF);
                ERRCHECK(result);
            }

            if (!rec->pcmData)
            {
                FMOD_SOUND_TYPE stype;
                FMOD_SOUND_FORMAT sformat;
                int channels_count;
                int bits_count;
                float freq;
                float volume;
                float pan;
                int priority;

                rec->base_sound->getDefaults(&freq, &volume, &pan, &priority);
                rec->base_sound->getFormat(&stype, &sformat, &channels_count, &bits_count);

                /*qDebug() << freq << " " << volume << " " << pan << " " << priority << " ";
                qDebug() << stype << " " << sformat << " " << channels_count << " " << bits_count << " ";*/

                rec->base_sound->getLength(&(rec->soundLenPcmBytes), FMOD_TIMEUNIT_PCMBYTES);
                rec->base_sound->seekData(0);

                qint8 *soundbuf = new qint8[rec->soundLenPcmBytes];
                rec->base_sound->readData((void*)soundbuf, rec->soundLenPcmBytes, &read);
                if (read>0) {
                    void *pcmData;
                    ConvertSoundBuffer(soundbuf,read,bits_count,channels_count,freq,&pcmData,&read);
                    if (read>0) {
                        rec->soundLenPcmBytes = read;
                        rec->pcmData = (qint16*) pcmData;
                    }
                }
                delete soundbuf;
            }
        }
    }
}

QString SoundList::getFunctionsText()
{
    QString result;
    GenSoundRecord *rec;
    int i = 0;

    foreach(rec, baseSoundsList)
    {
        if (!rec->sound_file.isEmpty() && !rec->sound_function.isEmpty())
        {
            i = baseSoundsList.indexOf(rec);
            result += "double " + rec->sound_function + "(double t) { return BaseSoundFunction("+QString::number(i)+", 3, t);} \r\n";
            result += "double " + rec->sound_function + "L(double t) { return BaseSoundFunction("+QString::number(i)+", 1, t);} \r\n";
            result += "double " + rec->sound_function + "R(double t) { return BaseSoundFunction("+QString::number(i)+", 2, t);} \r\n";
        }
    }

    return result;
}

double SoundList::playSound(int index, unsigned int channels, double t)
{
    double result = 0;

    if (baseSoundsList.size()>index) {
        GenSoundRecord *rec = baseSoundsList.data()[index];

        if (rec->base_sound && rec->pcmData && rec->soundLenPcmBytes)
        {
            unsigned int offsetL = t*44100.0*2;
            unsigned int offsetR = t*44100.0*2+1;

            if ((channels & 1) && offsetL<rec->soundLenPcmBytes) {
                result += rec->pcmData[offsetL]/32767.0;
            }
            if ((channels & 2) && offsetR<rec->soundLenPcmBytes) {
                result += rec->pcmData[offsetR]/32767.0;
            }
            if ((channels & 1) && (channels & 2) && offsetL<rec->soundLenPcmBytes && offsetR<rec->soundLenPcmBytes) {
                result *= 0.5;
            }
        }
    }

    return result;
}
