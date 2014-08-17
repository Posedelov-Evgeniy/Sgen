#include "soundlist.h"

SoundList::SoundList(AbstractSndController* base_controller)
{
    sc = base_controller;
}

void SoundList::clearSounds()
{
    GenSoundRecord *rec;
    FMOD_RESULT result;
    foreach(rec, baseSoundsList)
    {
        if (rec->base_sound) {
            result = rec->base_sound->release();
            AbstractSndController::ERRCHECK(result);
        }
        if (rec->pcmData) {
            delete[] rec->pcmData;
        }
        delete rec;
    }
    baseSoundsList.clear();
}

void SoundList::AddSound(QString new_file, QString new_function)
{
    GenSoundRecord *rec = new GenSoundRecord;

    rec->base_sound = 0;
    rec->pcmData = 0;
    rec->soundLenPcmBytes = 0;
    rec->soundLen = 0;
    rec->sound_function = new_function;
    rec->sound_file = new_file;
    rec->frequency = sc->getFrequency();
    rec->channels_count = sc->getChannelsCount();

    baseSoundsList.append(rec);
}

void SoundList::ConvertSoundBuffer(void *buf, int length, int bits_count, void **outbuf, unsigned int *outlength)
{
    *outlength = 0;
    if (bits_count>32 || bits_count<8) return;
    if (length<=0) return;
    if (!buf) return;

    int bytes_count = bits_count >> 3;
    int buf_elements = length/bytes_count;
    unsigned int outbuf_length = sizeof(qint32)*length/bytes_count;

    qint32 *maxbuff = new qint32[buf_elements];
    unsigned int i, j;

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
            maxbuff[i] = (qint32) intermediate*(max32bitval/max_cval);
        }
    }

    *outbuf = maxbuff;
    *outlength = outbuf_length;
}

void SoundList::InitSounds()
{
    if (!sc->getFmodSystem()) return;

    FMOD_RESULT result;
    unsigned int read = 0;
    GenSoundRecord *rec;
    foreach(rec, baseSoundsList)
    {
        if (!rec->sound_file.isEmpty() && !rec->sound_function.isEmpty())
        {
            if (!rec->base_sound)
            {
                result = sc->getFmodSystem()->createSound(qPrintable(rec->sound_file), FMOD_OPENONLY | FMOD_ACCURATETIME, 0, &(rec->base_sound));
                AbstractSndController::ERRCHECK(result);
                result = rec->base_sound->setMode(FMOD_LOOP_OFF);
                AbstractSndController::ERRCHECK(result);
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
                rec->base_sound->getLength(&(rec->soundLenPcmBytes), FMOD_TIMEUNIT_PCMBYTES);
                rec->base_sound->seekData(0);

                qint8 *soundbuf = new qint8[rec->soundLenPcmBytes];
                memset(soundbuf, 0, rec->soundLenPcmBytes);
                result = rec->base_sound->readData((void*)soundbuf, rec->soundLenPcmBytes, &read);
                AbstractSndController::ERRCHECK(result);
                if (read>0) {
                    void *pcmData;
                    ConvertSoundBuffer(soundbuf,read,bits_count,&pcmData,&read);
                    if (read>0) {
                        rec->soundLenPcmBytes = read;
                        rec->soundLen = read/sizeof(qint32);
                        rec->pcmData = (qint32*) pcmData;
                        rec->frequency = freq;
                        rec->channels_count = channels_count;
                    }
                }
                delete[] soundbuf;
            }
        }
    }
}

QString SoundList::getFunctionsText()
{
    QString result;
    GenSoundRecord *rec;
    int i = 0, j;

    InitSounds();
    foreach(rec, baseSoundsList)
    {
        if (rec->pcmData)
        {
            i = baseSoundsList.indexOf(rec);
            result += "double " + rec->sound_function + "(double t) { return BaseSoundFunction("+QString::number(i)+", 0, t);} \n";
            if (rec->channels_count>=2) {
                result += "double " + rec->sound_function + "_L(double t) { return BaseSoundFunction("+QString::number(i)+", 1, t);} \n";
                result += "double " + rec->sound_function + "_R(double t) { return BaseSoundFunction("+QString::number(i)+", 2, t);} \n";
            }
            for(j=0;j<rec->channels_count;j++) {
                result += "double " + rec->sound_function + "_" + QString::number(j)+"(double t) { return BaseSoundFunction("+QString::number(i)+", "+QString::number(j+1)+", t);} \n";
            }
        }
    }

    return result;
}

double SoundList::playSound(int index, unsigned int channel, double t)
{
    double result = 0;
    double max_val = std::numeric_limits<qint32>::max();

    if (baseSoundsList.size()>index) {
        GenSoundRecord *rec = baseSoundsList.data()[index];

        if (rec->base_sound && rec->pcmData && rec->soundLen)
        {
            unsigned int offset = ((unsigned int) (t*rec->frequency))*rec->channels_count;
            if (offset+rec->channels_count-1<rec->soundLen) {
                if (channel==0) {
                    for(unsigned int i=0;i<rec->channels_count;i++) {
                        result+=rec->pcmData[offset+i]/max_val;
                    }
                    result = result/rec->channels_count;
                } else if (channel<=rec->channels_count) {
                    result = rec->pcmData[offset+channel-1]/max_val;
                }
            }
        }
    }

    return result;
}
