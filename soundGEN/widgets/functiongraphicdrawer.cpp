#include "functiongraphicdrawer.h"
#include "ui_functiongraphicdrawer.h"

graphicThread* functionGraphicDrawer::mThread = 0;

functionGraphicDrawer::functionGraphicDrawer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::functionGraphicDrawer)
{
    ui->setupUi(this);

    widget_drawer = new MGraphicDrawSurface();
    widget_drawer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->verticalLayout->addWidget(widget_drawer);

    if (mThread==0) {
        mThread = new graphicThread();
    }

    mThread->addGraphic(this);

    widget_drawer->setT(0.001);
    widget_drawer->setT0(0.001);
    widget_drawer->setFreq(500);
    widget_drawer->setAmp(1);
    widget_drawer->setKt(0.005);
    widget_drawer->resetGraphicFunctions();

    on_durationSlider_valueChanged(ui->durationSlider->value());
    on_ampSlider_valueChanged(ui->ampSlider->value());
}

functionGraphicDrawer::~functionGraphicDrawer()
{
    mThread->removeGraphic();
    if (mThread->getLinksCount()==0) {
        mThread->Stop = true;
        mThread->quit();
        delete mThread;
        mThread = 0;
    }
    delete ui;
}

void functionGraphicDrawer::setGraphicFunction(GenSoundFunction value)
{
    widget_drawer->setGraphicFunction(value);
}

double functionGraphicDrawer::getT0() const
{
    return widget_drawer->getT0();
}

void functionGraphicDrawer::setT0(double value)
{
    widget_drawer->setT0(value);
}

double functionGraphicDrawer::getAmp() const
{
    return widget_drawer->getAmp();
}

void functionGraphicDrawer::setAmp(double value)
{
    widget_drawer->setAmp(value);
}

double functionGraphicDrawer::getFreq() const
{
    return widget_drawer->getFreq();
}

void functionGraphicDrawer::setFreq(double value)
{
    widget_drawer->setFreq(value);
}

int functionGraphicDrawer::getDtIntValue() const
{
    return ui->durationSlider->value();
}

void functionGraphicDrawer::setDtIntValue(int value)
{
    ui->durationSlider->setValue(value);
    on_durationSlider_valueChanged(value);
}

int functionGraphicDrawer::getKampIntValue() const
{
    return ui->ampSlider->value();
}

void functionGraphicDrawer::setKampIntValue(int value)
{
    ui->ampSlider->setValue(value);
    on_ampSlider_valueChanged(value);
}

void functionGraphicDrawer::drawCycle()
{
    widget_drawer->incT();
    widget_drawer->update();
    ui->lcdNumber_t->display(widget_drawer->getT());
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
    widget_drawer->setDt(0.1 * value / ui->durationSlider->maximum());
    ui->lcdNumber_dur->display(widget_drawer->getDt());
    emit changed();
}

void functionGraphicDrawer::on_ampSlider_valueChanged(int value)
{
    widget_drawer->setKamp(1.0 + (double) 2*value / ui->ampSlider->maximum());
    ui->lcdNumber_koef->display(widget_drawer->getKamp());
    emit changed();
}
