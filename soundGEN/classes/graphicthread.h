#ifndef GRAPHICTHREAD_H
#define GRAPHICTHREAD_H

#include <QThread>

class graphicThread : public QThread
{
    Q_OBJECT
public:
    explicit graphicThread(QObject *parent = 0);
    void run();
    void addGraphic(QObject *graphicDrawer);
    void removeGraphic();
    bool Stop;
    int getLinksCount() const;
    unsigned long getInterval() const;
    void setInterval(unsigned long value);
private:
    int linksCount;
    unsigned long interval;
signals:
    void DrawStep();
public slots:
};

#endif // GRAPHICTHREAD_H
