#include "graphicthread.h"
#include <QtCore>
#include <QtGui>

graphicThread::graphicThread(QObject *parent) :
    QThread(parent)
{
    Stop = false;
    linksCount = 0;
}

void graphicThread::run()
{
    do {
        emit DrawStep();
        this->msleep(80);
    } while (!Stop);
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

