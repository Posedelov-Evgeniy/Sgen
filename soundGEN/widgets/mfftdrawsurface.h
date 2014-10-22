#ifndef MFFTDRAWSURFACE_H
#define MFFTDRAWSURFACE_H

#include "mgraphicdrawsurface.h"
#include "../classes/sndanalyzer.h"

class MFftDrawSurface : public MGraphicDrawSurface
{
    Q_OBJECT
private:
    QVector<HarmonicInfo>* data;
    QVector<HarmonicInfo>* data_buffer;
    double* draw_result = 0;
    double last_dt = 0;
    unsigned int draw_size = 0;
public:
    explicit MFftDrawSurface();
protected:
    virtual void incT();
    virtual void paintEvent(QPaintEvent* e);
};

#endif // MFFTDRAWSURFACE_H
