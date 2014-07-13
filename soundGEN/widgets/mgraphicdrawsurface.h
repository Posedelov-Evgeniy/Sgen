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
private:
    double t, t0, amp, freq, dt, kt, kamp, dt_axis;
    GenSoundFunction graphicFunction;
    void calculateTGrid();
public:
    explicit MGraphicDrawSurface();

    double getT() const;
    void setT(double value);

    double getT0() const;
    void setT0(double value);

    double getAmp() const;
    void setAmp(double value);

    double getFreq() const;
    void setFreq(double value);

    double getDt() const;
    void setDt(double value);

    double getKt() const;
    void setKt(double value);

    double getKamp() const;
    void setKamp(double value);

    double getDt_axis() const;

    void incT();

    void setGraphicFunction(const GenSoundFunction &value);

protected:
    virtual void paintEvent(QPaintEvent* e);
};

#endif // MGRAPHICDRAWSURFACE_H
