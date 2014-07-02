#include "graphicthread.h"
#include <QtCore>
#include <QtGui>

graphicThread::graphicThread(QObject *parent) :
    QThread(parent)
{
    Stop = false;
}

void graphicThread::run()
{
    do {
        emit DrawStep();
        this->msleep(100);
    } while (!Stop);
}
