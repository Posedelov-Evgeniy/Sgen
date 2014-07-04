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
                rec->base_sound->getLength(&(rec->soundLenPcmBytes), FMOD_TIMEUNIT_PCMBYTES);
                rec->base_sound->seekData(0);

                printf("PCM BYTES: %d\r", rec->soundLenPcmBytes);

                rec->pcmData = new signed short[rec->soundLenPcmBytes];
                rec->base_sound->readData(rec->pcmData, rec->soundLenPcmBytes, &read);
                if (read<rec->soundLenPcmBytes) {
                    rec->soundLenPcmBytes = read;
                }

                printf("READ: %d\r", rec->soundLenPcmBytes);
                fflush(stdout);
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
