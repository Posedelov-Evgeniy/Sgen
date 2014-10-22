#include "graphicthread.h"
#include <QtCore>
#include <QtGui>

graphicThread::graphicThread(QObject *parent) :
    QThread(parent)
{
    Stop = false;
    linksCount = 0;
    interval = 30;
}

void graphicThread::run()
{
    do {
        emit DrawStep();
        msleep(interval);
    } while (!Stop);
    emit DrawStep();
}

void graphicThread::addGraphic(QObject *graphicDrawer)
{
    linksCount++;
    connect(this, SIGNAL(DrawStep()), graphicDrawer, SLOT(drawCycle()));
}

void graphicThread::removeGraphic()
{
    if (linksCount>0) linksCount--;
}

int graphicThread::getLinksCount() const
{
    return linksCount;
}

unsigned long graphicThread::getInterval() const
{
    return interval;
}

void graphicThread::setInterval(unsigned long value)
{
    interval = value;
}


