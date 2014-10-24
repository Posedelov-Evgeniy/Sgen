#include "sndanalyzer.h"

SndAnalyzer::SndAnalyzer()
{
    top_harmonic = 4;
    amp_filter = 0;
    skip_zero_frequency = true;
    harmonics = new QVector<HarmonicInfo>;
    top_harmonics = new QVector<HarmonicInfo>;
}

SndAnalyzer::~SndAnalyzer()
{
    delete harmonics;
    delete top_harmonics;
}

void SndAnalyzer::function_fft_top_only(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points)
{
    double t, dt;
    unsigned int i;

    result_amp = result_freq = 0;

    kiss_fft_cpx* cin = new kiss_fft_cpx[points];
    kiss_fft_cpx* cout = new kiss_fft_cpx[points];
    kiss_fft_cfg  kiss_fft_state;
    kiss_fft_scalar zero;
    memset(&zero,0,sizeof(zero) );

    dt = (t2-t1)/points;
    for(i = 0; i<points; i++) {
        t = t1 + dt*i;
        cin[i].i = zero;
        cin[i].r = fct(t, freq*2*M_PI, freq, pfct);
        if (abs(cin[i].r)>result_amp) result_amp = abs(cin[i].r);
    }

    kiss_fft_state = kiss_fft_alloc(points,0,0,0);
    kiss_fft(kiss_fft_state,cin,cout);

    function_fft_calc_top(cout, points, abs(t2-t1));

    free(kiss_fft_state);
    delete [] cin;
    delete [] cout;
}

void SndAnalyzer::function_fft_base(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points)
{
    double t, dt;
    double timelen = abs(t2-t1);
    unsigned int i, j, i_start, i_finish;
    double base_freq = points / timelen;

    i_start = 0;
    i_finish = points - 1;

    if (skip_zero_frequency) {
        i_start++;
        i_finish--;
    }

    result_amp = result_freq = 0;

    kiss_fft_cpx* cin = new kiss_fft_cpx[points];
    kiss_fft_cpx* cout = new kiss_fft_cpx[points];
    kiss_fft_cfg  kiss_fft_state;
    kiss_fft_scalar zero;
    memset(&zero,0,sizeof(zero) );

    dt = (t2-t1)/points;
    for(i = 0; i<points; i++) {
        t = t1 + dt*i;
        cin[i].i = zero;
        cin[i].r = fct(t, freq*2*M_PI, freq, pfct);
        if (abs(cin[i].r)>result_amp) result_amp = abs(cin[i].r);
    }

    kiss_fft_state = kiss_fft_alloc(points,0,0,0);
    kiss_fft(kiss_fft_state,cin,cout);

    double tmp_amp, tmp_amp2, tmp_freq;
    HarmonicInfo tmp_info;

    harmonics->clear();

    for(i = i_start, j = i_finish; i<=j; i++, j--) {
        tmp_amp = sqrt(sqr(cout[i].r)+sqr(cout[i].i)) / base_freq;
        tmp_amp2 = sqrt(sqr(cout[j].r)+sqr(cout[j].i)) / base_freq;
        if (tmp_amp<tmp_amp2) tmp_amp = tmp_amp2;
        tmp_freq = i/timelen;
        tmp_info.amp = tmp_amp;
        tmp_info.freq = tmp_freq;
        harmonics->append(tmp_info);
    }

    function_fft_calc_top(cout, points, timelen);

    free(kiss_fft_state);
    delete [] cin;
    delete [] cout;
}

unsigned int SndAnalyzer::getTop_harmonic() const
{
    return top_harmonic;
}

void SndAnalyzer::setTop_harmonic(unsigned int value)
{
    top_harmonic = value;
}

bool SndAnalyzer::getSkip_zero_frequency() const
{
    return skip_zero_frequency;
}

void SndAnalyzer::setSkip_zero_frequency(bool value)
{
    skip_zero_frequency = value;
}

QVector<HarmonicInfo> *SndAnalyzer::getHarmonics()
{
    return harmonics;
}

void SndAnalyzer::clearHarmonics()
{
    harmonics->clear();
}

QVector<HarmonicInfo> *SndAnalyzer::getTopHarmonics()
{
    return top_harmonics;
}

void SndAnalyzer::clearTopHarmonics()
{
    top_harmonics->clear();
}
double SndAnalyzer::getAmp_filter() const
{
    return amp_filter;
}

void SndAnalyzer::setAmp_filter(double value)
{
    amp_filter = value;
}


void SndAnalyzer::function_fft_calc_top(kiss_fft_cpx *cout, unsigned int points, double timelen)
{
    unsigned int mi;
    bool mi_set;
    double max_amp, max_freq, tmp_amp, tmp_amp2, tmp_freq;
    HarmonicInfo tmp_info;

    unsigned int i, j, i_start, i_finish;
    double base_freq = points / timelen;

    i_start = 0;
    i_finish = points-1;

    if (skip_zero_frequency) {
        i_start++;
        i_finish--;
    }

    top_harmonics->clear();

    do {
        mi_set = false;
        max_amp = 0;
        max_freq = 0;
        for(i = i_start, j = i_finish; i<=j; i++, j--) {
            tmp_amp = sqrt(sqr(cout[i].r)+sqr(cout[i].i)) / base_freq;
            tmp_amp2 = sqrt(sqr(cout[j].r)+sqr(cout[j].i)) / base_freq;
            if (tmp_amp<tmp_amp2) tmp_amp = tmp_amp2;
            if (max_amp<=tmp_amp) {
                tmp_freq = i/timelen;
                mi = i;
                mi_set = true;
                max_amp = tmp_amp;
                max_freq = tmp_freq;
            }
        }
        if (mi_set && max_amp<amp_filter) mi_set = false;
        if (mi_set) {
            tmp_info.amp = max_amp;
            tmp_info.freq = max_freq;
            top_harmonics->append(tmp_info);
            cout[mi].r = cout[points-1-mi].r = 0;
            cout[mi].i = cout[points-1-mi].i = 0;
        }

    } while (top_harmonics->count()<top_harmonic && mi_set);

    if (top_harmonics->length()>0) {
        result_freq = top_harmonics->at(0).freq;
    }
}

double SndAnalyzer::getInstFrequency()
{
    return result_freq;
}

double SndAnalyzer::getInstAmp()
{
    return result_amp;
}
