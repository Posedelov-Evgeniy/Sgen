#include "functiongraphicdrawer.h"
#include "ui_functiongraphicdrawer.h"

functionGraphicDrawer::functionGraphicDrawer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::functionGraphicDrawer)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    mThread = new graphicThread(this);
    connect(mThread, SIGNAL(DrawStep()), this, SLOT(drawCycle()));

    t = t0 = 0;
    amp = 1;
    freq = 500;
    dt = 1;
    kamp = 1;
    kt = 0.05;
    graphicFunction = 0;
}

functionGraphicDrawer::~functionGraphicDrawer()
{
    delete mThread;
    delete ui;
}

void functionGraphicDrawer::setGraphicFunction(const GenSoundFunction &value)
{
    graphicFunction = value;
}

double functionGraphicDrawer::getT0() const
{
    return t0;
}

void functionGraphicDrawer::setT0(double value)
{
    t0 = value;
}

double functionGraphicDrawer::getAmp() const
{
    return amp;
}

void functionGraphicDrawer::setAmp(double value)
{
    amp = value;
}

double functionGraphicDrawer::getFreq() const
{
    return freq;
}

void functionGraphicDrawer::setFreq(double value)
{
    freq = value;
}

void functionGraphicDrawer::drawCycle()
{
    QPen pen(Qt::red);
    int points_count = ui->graphicsView->width() / 2 - 1;
    int height_center = ui->graphicsView->height() / 2;
    double k_y_graphic = kamp * 0.375 * ui->graphicsView->height();
    double k_t_graphic = dt/points_count;
    int i;
    double x0, y0, x1, y1;

    scene->clear();
    x1 = 0;
    y1 = height_center - k_y_graphic*graphicFunction(t, amp, freq, base_play_sound);
    for(i=1;i<points_count;i++) {
        x0 = x1;
        y0 = y1;
        x1 = i*2;
        y1 = height_center - k_y_graphic*graphicFunction(t+i*k_t_graphic, amp, freq, base_play_sound);
        scene->addLine(x0,y0,x1,y1,pen);
    }
    t += kt*dt;
}


void functionGraphicDrawer::run()
{
    mThread->Stop = false;
    mThread->start();
}

void functionGraphicDrawer::stop()
{
    mThread->Stop = true;
}


void functionGraphicDrawer::on_durationSlider_valueChanged(int value)
{
    dt = (double) 2*value / ui->durationSlider->maximum();
}

void functionGraphicDrawer::on_ampSlider_valueChanged(int value)
{
    kamp = 1.0 + (double) 2*value / ui->durationSlider->maximum();
}
