#include "mfftdrawsurface.h"

MFftDrawSurface::MFftDrawSurface() :
    MGraphicDrawSurface()
{
    timer_interval = 30;
    draw_result = 0;
    draw_size = 0;
    last_fmod_dt = -1;
    t = 0;
    dt = 0;
    next_dt = 0.5;
    data = data_buffer = data_top = 0;
    round_interval_dt = ceil(timer_interval*0.001);
    grid_k = 0.5;
    max_y = 0;
    max_y_axis = 1;
    analyzer = new SndAnalyzer();
    analyzer->setTop_harmonic(5);
    analyzer->setAmp_filter(0.0001);
}

MFftDrawSurface::~MFftDrawSurface()
{
    if (data)     delete data;
    if (data_top) delete data_top;
    delete analyzer;
}

void MFftDrawSurface::setTimerInterval(long int interval)
{
    timer_interval = interval;
}

double MFftDrawSurface::getDt() const
{
    return next_dt;
}

void MFftDrawSurface::setDt(double value)
{
    next_dt = value;
}

void MFftDrawSurface::recalcData()
{
    double cfmod = fmod(t, round_interval_dt);
    if (last_fmod_dt>cfmod && graphicFunction) {
        if (data) {
            delete data;
            data = 0;
        }
        if (data_top) {
            delete data_top;
            data_top = 0;
        }
        if (!analyzer->getHarmonics() || analyzer->getHarmonics()->isEmpty()) {
            analyzer->function_fft_base(graphicFunction, t, t+dt, freq, floor(dt*SndController::Instance()->getFrequency()));
        }
        if (analyzer->getHarmonics()) {
            data = new QVector<HarmonicInfo>(*(analyzer->getHarmonics()));
        }
        if (analyzer->getTopHarmonics()) {
            data_top = new QVector<HarmonicInfo>(*(analyzer->getTopHarmonics()));
        }
        analyzer->function_fft_base(graphicFunction, t+dt, t+2*dt, freq, floor(dt*SndController::Instance()->getFrequency()));
        data_buffer = analyzer->getHarmonics();
    }
    last_fmod_dt = cfmod;
}

void MFftDrawSurface::incT()
{
    t = SndController::Instance()->getT();
    if (this->isHidden()) return;
    if (dt!=next_dt) {
        dt = next_dt;
        round_interval_dt = ceil((ceil((dt*500)/timer_interval) / 1000.0) * timer_interval);
        last_fmod_dt = -1;
        analyzer->clearHarmonics();
    }
    recalcData();
}

void MFftDrawSurface::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (this->isHidden()) return;

    QPainter painter(this);
    painter.setBackground(QBrush(Qt::white));
    painter.setPen(Qt::black);
    painter.drawRect(rect().left(),rect().top(),rect().right()-1,rect().bottom()-1);
    if (!data || data->isEmpty()) return;
    if (!data_buffer || data_buffer->isEmpty()) return;
    if (last_fmod_dt<0) return;

    if (draw_size!=width()-10 && draw_result) {
        delete [] draw_result;
        draw_result = 0;
        draw_size = 0;
    }
    if (!draw_result) {
        draw_size = width()-10;
        draw_result = new double[draw_size];
    }

    QFontMetrics fm(painter.font());
    int height_center = height()-fm.height()-5;

    unsigned int i, j;
    double x0, y0, x1, y1;

    double f0 = 0;
    double fc = f0;
    double f1 = data->last().freq;
    double f_axis = calculateTGrid(f1);
    unsigned int pixel_count = floor(data->size()/draw_size);
    if (pixel_count<=0) pixel_count = 1;

    for(i=0;i<draw_size;i++) {
        draw_result[i] = 0;
        for(j=0;j<pixel_count;j++) {
            y0 = (1-last_fmod_dt/round_interval_dt) * data->at(i*pixel_count+j).amp + (last_fmod_dt/round_interval_dt) * data_buffer->at(i*pixel_count+j).amp;
            if (y0>draw_result[i]) {
                draw_result[i] = y0;
            }
        }
        if (draw_result[i]>max_y) {
            max_y = draw_result[i];
            max_y_axis = -1;
        }
    }

    if (max_y_axis<0) {
        max_y_axis = calculateTGrid(max_y);
    }

    double k_y = 1;
    if (max_y>0 && max_y<1) k_y = 1/max_y;

    painter.drawLine(0,height_center,draw_size+10,height_center);

    y0 = max_y_axis;
    do {
        y1 = height_center * (1 - 0.95*k_y*y0);
        QString axis_num = QString::number(round(y0*1000)/1000);
        painter.drawLine(0, y1,7,y1);
        painter.drawText(10, y1+fm.height()/2, axis_num);
        y0+=max_y_axis;
    } while (y0<=max_y);

    do {
        QString axis_num = QString::number(round(fc*10)/10);
        x0 = (fc/f1) * draw_size + 5;
        painter.drawLine(x0,0,x0,height_center+2);
        painter.drawText(x0-0.5*fm.width(axis_num)+1, height_center+fm.height()+1, axis_num);
        fc+=f_axis;
    } while (fc<=f1);

    painter.setPen(QPen(QBrush(Qt::red), 2));

    x1 = 5;
    y1 = height_center * (1 - draw_result[0]);
    for(i=1;i<draw_size;i++) {
        x0 = x1;
        y0 = y1;
        x1 = i+5;
        y1 = height_center * (1 - 0.95*k_y*draw_result[i]);
        painter.drawLine(x0,y0,x1,y1);
    }

    if (data_top && !data_top->isEmpty()) {
        int max_top_width = fm.width("00000.0"+tr("Hz")+" -> 0.0000");
        int tmp_top_width = fm.width(tr("Top harmonics:"));
        if (tmp_top_width>max_top_width) max_top_width = tmp_top_width;
        max_top_width+=15;
        painter.setBrush(QColor(0, 0, 255, 150));
        painter.setPen(Qt::blue);
        painter.drawRect(rect().right()-10-max_top_width,rect().top()+10,max_top_width,10+(data_top->size()+1)*(fm.height()+5));
        painter.setPen(Qt::yellow);
        painter.drawText(rect().right()-max_top_width, rect().top()+25, tr("Top harmonics:"));
        painter.setPen(Qt::white);
        for(i=0;i<data_top->size();i++) {
            painter.drawText(rect().right()-max_top_width, rect().top()+30+(i+1)*(fm.height()+5), QString::number(data_top->at(i).freq, 'f', 1) + tr("Hz") + " -> " + QString::number(data_top->at(i).amp, 'f', 4));
        }
    }
}

void MFftDrawSurface::mousePressEvent(QMouseEvent *event)
{
    return;
}

void MFftDrawSurface::mouseReleaseEvent(QMouseEvent *event)
{
    return;
}

void MFftDrawSurface::mouseMoveEvent(QMouseEvent *event)
{
    return;
}

void MFftDrawSurface::wheelEvent(QWheelEvent *event)
{
    return;
}
