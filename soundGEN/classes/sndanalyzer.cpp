#include "sndanalyzer.h"

SndAnalyzer::SndAnalyzer()
{
    top_harmonic = 4;
    skip_zero_frequency = true;
    harmonics = new QVector<HarmonicInfo>;
}

SndAnalyzer::~SndAnalyzer()
{
    delete harmonics;
}

void SndAnalyzer::function_fft(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points)
{
    double t, dt;
    double timelen = abs(t2-t1);
    unsigned int i, i_start, i_finish;
    double base_freq = points / timelen;

    i_start = 0;
    i_finish = points-1;

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

    unsigned int mi;
    bool mi_set;
    double max_amp, max_freq, tmp_amp, tmp_freq;
    HarmonicInfo tmp_info;

    harmonics->clear();

    do {
        mi_set = false;
        max_amp = 0;
        max_freq = 0;
        for(i = i_start; i<=i_finish; i++) {
            tmp_amp = 2 * sqrt(sqr(cout[i].r)+sqr(cout[i].i)) / base_freq;
            if (max_amp<=tmp_amp) {
                tmp_freq = i <= base_freq/2 ? i/timelen : base_freq-i/timelen;
                if (max_freq<tmp_freq) {
                    mi = i;
                    mi_set = true;
                    max_amp = tmp_amp;
                    max_freq = tmp_freq;
                }
            }
        }
        if (mi_set) {
            tmp_info.amp = max_amp;
            tmp_info.freq = max_freq;
            harmonics->append(tmp_info);
            cout[mi].r = 0;
            cout[mi].i = 0;
        }

    } while (harmonics->count()<top_harmonic && mi_set);

    if (harmonics->length()>0) {
        result_freq = harmonics->at(0).freq;
    }

    free(kiss_fft_state);
    delete [] cin;
    delete [] cout;
}

void SndAnalyzer::function_fft_for_graph(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points)
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
        tmp_amp = 2 * sqrt(sqr(cout[i].r)+sqr(cout[i].i)) / base_freq;
        tmp_amp2 = 2 * sqrt(sqr(cout[j].r)+sqr(cout[j].i)) / base_freq;
        if (tmp_amp<tmp_amp2) tmp_amp = tmp_amp2;
        tmp_freq = i/timelen;
        tmp_info.amp = tmp_amp;
        tmp_info.freq = tmp_freq;
        harmonics->append(tmp_info);
    }

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

double SndAnalyzer::getInstFrequency()
{
    return result_freq;
}

double SndAnalyzer::getInstAmp()
{
    return result_amp;
}
