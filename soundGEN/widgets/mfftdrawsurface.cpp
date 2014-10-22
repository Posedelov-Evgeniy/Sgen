#include "mfftdrawsurface.h"

MFftDrawSurface::MFftDrawSurface() :
    MGraphicDrawSurface()
{
    draw_result = 0;
    draw_size = 0;
    data = new QVector<HarmonicInfo>;
}

void MFftDrawSurface::incT()
{
    t = SndController::Instance()->getT();
}

void MFftDrawSurface::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (this->isHidden()) return;

    QPainter painter(this);
    painter.setBackground(QBrush(Qt::white));
    painter.setPen(Qt::black);
    painter.drawRect(rect().left(),rect().top(),rect().right()-1,rect().bottom()-1);
    if (data->isEmpty()) return;

    if (draw_size!=width() && draw_result) {
        delete [] draw_result;
        draw_result = 0;
        draw_size = 0;
    }
    if (!draw_result) {
        draw_size = width();
        draw_result = new double[draw_size];
    }

    last_dt = dt;
    int height_center = height()-15;
    double kFreq = freq*2.0*M_PI;

    int i;
    double x0, y0, x1, y1;

    double f0 = 0;
    double fc = f0;
    double f1 = data->size();
    double f_axis = calculateTGrid(f1);
    double f_k_axis = 0;
    unsigned int pixel_count = floor(data->size()/draw_size);

    painter.drawLine(0,height_center,draw_size,height_center);

    do {
        x0 = (fc/f1) * draw_size;
        painter.drawLine(x0,0,x0,height_center+2);
        painter.drawText(x0+5, height_center+5, QString::number(round(f_axis*100000)/100000));
        fc+=f_axis;
    } while (fc<=f1);

    painter.setPen(QPen(QBrush(Qt::red), 2));

    /*x1 = 0;
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
    }*/
}
