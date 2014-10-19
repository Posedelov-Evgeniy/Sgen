#include "sndanalyzer.h"

SndAnalyzer::SndAnalyzer()
{
    top_harmonic = 4;
    harmonics = new QVector<HarmonicInfo>;
}

SndAnalyzer::~SndAnalyzer()
{
    delete harmonics;
}

void SndAnalyzer::function_fft(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points, double timelen)
{
    double t, dt;
    unsigned int i;
    double base_freq = points / timelen;

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
    double max_amp, tmp_amp;
    HarmonicInfo tmp_info;

    harmonics->clear();

    do {
        mi_set = false;
        max_amp = 0;
        for(i = 0; i<points; i++) {
            tmp_amp = sqrt(sqr(cout[i].r)+sqr(cout[i].i));
            if (max_amp<tmp_amp) {
                mi = i;
                mi_set = true;
                max_amp = tmp_amp;
            }
        }
        if (mi_set) {
            tmp_info.amp = max_amp / (0.5*base_freq);
            tmp_info.freq = mi <= base_freq/2 ? mi/timelen : base_freq-mi/timelen;
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

unsigned int SndAnalyzer::getTop_harmonic() const
{
    return top_harmonic;
}

void SndAnalyzer::setTop_harmonic(unsigned int value)
{
    top_harmonic = value;
}

double SndAnalyzer::getInstFrequency()
{
    return result_freq;
}

double SndAnalyzer::getInstAmp()
{
    return result_amp;
}
