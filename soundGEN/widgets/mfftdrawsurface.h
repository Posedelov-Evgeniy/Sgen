#ifndef MFFTDRAWSURFACE_H
#define MFFTDRAWSURFACE_H

#include "mgraphicdrawsurface.h"
#include "../classes/sndanalyzer.h"

class MFftDrawSurface : public MGraphicDrawSurface
{
    Q_OBJECT
private:
    SndAnalyzer *analyzer;
    QVector<HarmonicInfo>* data;
    QVector<HarmonicInfo>* data_buffer;
    double* draw_result;
    double last_fmod_dt;
    double round_interval_dt;
    double next_dt;
    double max_y, max_y_axis;
    unsigned int draw_size;
    long int timer_interval;
    void recalcData();
public:
    explicit MFftDrawSurface();
    ~MFftDrawSurface();
    void setTimerInterval(long int interval);
    virtual double getDt() const;
    void setDt(double value);
    void incT();
protected:
    virtual void paintEvent(QPaintEvent* e);
};

#endif // MFFTDRAWSURFACE_H
