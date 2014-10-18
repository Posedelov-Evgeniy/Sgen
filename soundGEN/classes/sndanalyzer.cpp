#include "sndanalyzer.h"

SndAnalyzer::SndAnalyzer()
{
    top_harmonic = 10;
}

void SndAnalyzer::fft_results(kiss_fft_cpx* cout, unsigned int size)
{
    unsigned int i;
    unsigned int mi;
    double max_amp = 0, ree = 0, imm = 0;
    double tmp_amp;

    for(i = 0; i<size; i++) {
        tmp_amp = sqrt(sqr(cout[i].r)+sqr(cout[i].i));
        if (max_amp<tmp_amp) {
            mi = i;
            ree = cout[i].r;
            imm = cout[i].i;
            max_amp = tmp_amp;
        }
    }

    //qDebug() << mi << (float) max_amp << (float) ree << (float) imm;
}

void SndAnalyzer::function_fft(GenSoundFunction fct, PlaySoundFunction pfct, double t1, double t2, double freq, unsigned int points, double timelen)
{
    double t, dt;
    unsigned int i;

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
    }

    kiss_fft_state = kiss_fft_alloc(points,0,0,0);
    kiss_fft(kiss_fft_state,cin,cout);

    fft_results(cout, points);

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
