#ifndef GRAPHICTHREAD_H
#define GRAPHICTHREAD_H

#include <QThread>

class graphicThread : public QThread
{
    Q_OBJECT
public:
    explicit graphicThread(QObject *parent = 0);
    void run();
    bool Stop;
signals:
    void DrawStep();
public slots:
};

#endif // GRAPHICTHREAD_H
