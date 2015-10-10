#ifndef SNDANALYZER_H
#define SNDANALYZER_H

#include <math.h>
#include <QVector>
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
    void function_fft_top_only(GenSignalFunction fct, double t1, double t2, double freq, unsigned int points);
    void function_fft_base(GenSignalFunction fct, double t1, double t2, double freq, unsigned int points);
    double getInstFrequency();
    double getInstAmp();
    unsigned int getTop_harmonic() const;
    void setTop_harmonic(unsigned int value);
    bool getSkip_zero_frequency() const;
    void setSkip_zero_frequency(bool value);
    QVector<HarmonicInfo>* getHarmonics();
    void clearHarmonics();
    QVector<HarmonicInfo> *getTopHarmonics();
    void clearTopHarmonics();
    double getAmp_filter() const;
    void setAmp_filter(double value);
private:
    double result_freq, result_amp;
    double amp_filter;
    bool skip_zero_frequency;
    unsigned int top_harmonic;
    QVector<HarmonicInfo>* harmonics;
    QVector<HarmonicInfo>* top_harmonics;
    void function_fft_calc_top(kiss_fft_cpx* cout, unsigned int points, double timelen);
};

#endif // SNDANALYZER_H
