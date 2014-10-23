#include "mgraphicdrawsurface.h"


double MGraphicDrawSurface::getT() const
{
    return t;
}

void MGraphicDrawSurface::setT(double value)
{
    t = value;
}

double MGraphicDrawSurface::getT0() const
{
    return t0;
}

void MGraphicDrawSurface::setT0(double value)
{
    t0 = value;
}

double MGraphicDrawSurface::getAmp() const
{
    return amp;
}

void MGraphicDrawSurface::setAmp(double value)
{
    amp = value;
}

double MGraphicDrawSurface::getFreq() const
{
    return freq;
}

void MGraphicDrawSurface::setFreq(double value)
{
    freq = value;
}

double MGraphicDrawSurface::getDt() const
{
    return dt;
}

void MGraphicDrawSurface::setDt(double value)
{
    dt = value;
    dt_axis = calculateTGrid(dt);
}

double MGraphicDrawSurface::getKt() const
{
    return kt;
}

void MGraphicDrawSurface::setKt(double value)
{
    kt = value;
}

double MGraphicDrawSurface::getKamp() const
{
    return kamp;
}

void MGraphicDrawSurface::setKamp(double value)
{
    kamp = value;
}

double MGraphicDrawSurface::getDt_axis() const
{
    return dt_axis;
}

void MGraphicDrawSurface::incT()
{
    t += kt*dt;
}

void MGraphicDrawSurface::setGraphicFunction(GenSoundFunction value)
{
    graphicFunction = value;
    graphicTFunction = 0;
}

void MGraphicDrawSurface::setGraphicFunctionT(base_function_signal value)
{
    graphicTFunction = value;
    graphicFunction = 0;
}

void MGraphicDrawSurface::resetGraphicFunctions()
{
    graphicTFunction = 0;
    graphicFunction = 0;
}

MGraphicDrawSurface::MGraphicDrawSurface() :
    QWidget()
{
    grid_k = 1;
}

double MGraphicDrawSurface::calculateTGrid(double cl_dt)
{
    double dt_ax = 0;

    if (cl_dt>0)
    {
        double divider = 1;
        while (cl_dt<1) {
            divider *= 10;
            cl_dt *= 10;
        }
        while (cl_dt>10) {
            divider *= 0.1;
            cl_dt *= 0.1;
        }
        cl_dt = round(cl_dt);
        if (cl_dt>5) dt_ax = 5/divider;
        else if(cl_dt>2) dt_ax = 2/divider;
        else if(cl_dt>1) dt_ax = 1/divider;
        else dt_ax = 0.5/divider;

        dt_ax = grid_k * dt_ax;
    } else
    {
        dt_ax = 1;
    }
    return dt_ax;
}

void MGraphicDrawSurface::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (this->isHidden()) return;

    QPainter painter(this);
    painter.setBackground(QBrush(Qt::white));
    painter.setPen(Qt::black);
    painter.drawRect(rect().left(),rect().top(),rect().right()-1,rect().bottom()-1);
    if (!graphicFunction && !graphicTFunction) return;

    int points_count = 2 * width() - 1;
    int height_center = height() / 2;
    double k_y_graphic = kamp * amp * 0.4 * height();
    double k_t_graphic = dt/points_count;
    double kFreq = freq*2.0*M_PI;

    int i;
    double x0, y0, x1, y1;

    double t_axis = floor(t/dt_axis) * dt_axis;
    double next_t = t+dt;


    painter.drawLine(0,height_center,points_count,height_center);

    do {
        t_axis+=dt_axis;
        x0 = 0.5*(t_axis-t)/k_t_graphic;
        painter.drawLine(x0,0,x0,height_center*2);
        painter.drawText(x0+5, height_center+5, QString::number(round(t_axis*100000)/100000));
        painter.drawText(x0+5, 15, QString::number(round(t_axis*100000)/100000));
    } while (t_axis<=next_t);

    painter.setPen(QPen(QBrush(Qt::red), 2));

    x1 = 0;
    if (graphicFunction)
        y1 = height_center - k_y_graphic*graphicFunction(t, kFreq, freq, base_play_sound);
    else
        y1 = height_center - k_y_graphic*graphicTFunction(kFreq*t);

    for(i=1;i<points_count;i++) {
        x0 = x1;
        y0 = y1;
        x1 = i/2;

        if (graphicFunction)
            y1 = height_center - k_y_graphic*graphicFunction(t+i*k_t_graphic, kFreq, freq, base_play_sound);
        else
            y1 = height_center - k_y_graphic*graphicTFunction((t+i*k_t_graphic)*kFreq);
        painter.drawLine(x0,y0,x1,y1);
    }
}
