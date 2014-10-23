#ifndef MGRAPHICDRAWSURFACE_H
#define MGRAPHICDRAWSURFACE_H

#include <QWidget>
#include <QPainter>
#include <math.h>
#include "../base_functions.h"
#include "../sndcontroller.h"

class MGraphicDrawSurface : public QWidget
{
    Q_OBJECT
protected:
    double t, t0, amp, freq, dt, kt, kamp, dt_axis;
    double grid_k;
    GenSoundFunction graphicFunction;
    base_function_signal graphicTFunction;
    double calculateTGrid(double cl_dt);
public:
    explicit MGraphicDrawSurface();

    virtual double getT() const;
    virtual void setT(double value);

    double getT0() const;
    void setT0(double value);

    double getAmp() const;
    void setAmp(double value);

    double getFreq() const;
    void setFreq(double value);

    virtual double getDt() const;
    virtual void setDt(double value);

    double getKt() const;
    void setKt(double value);

    double getKamp() const;
    void setKamp(double value);

    double getDt_axis() const;

    virtual void incT();

    void setGraphicFunction(GenSoundFunction value);
    void setGraphicFunctionT(base_function_signal value);
    void resetGraphicFunctions();

protected:
    virtual void paintEvent(QPaintEvent* e);
};

#endif // MGRAPHICDRAWSURFACE_H
