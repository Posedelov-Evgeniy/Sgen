#ifndef SNDANALYZER_H
#define SNDANALYZER_H

#include <math.h>
#include <QVector>
#include <qDebug>
#include "../abstractsndcontroller.h"
#include "../kiss_fft/kiss_fftr.h"
#include "../kiss_fft/_kiss_fft_guts.h"

struct HarmonicInfo {
    double freq;
    double amp;
};

class SndAnalyzer
{
public:
    SndAnalyzer();
    ~SndAnalyzer();
    void function_fft(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points, double timelen);
    double getInstFrequency();
    double getInstAmp();
    unsigned int getTop_harmonic() const;
    void setTop_harmonic(unsigned int value);
private:
    double result_freq, result_amp;
    unsigned int top_harmonic;
    QVector<HarmonicInfo>* harmonics;
};

#endif // SNDANALYZER_H
