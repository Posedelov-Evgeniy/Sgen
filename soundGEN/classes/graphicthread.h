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
private:
    int linksCount;
signals:
    void DrawStep();
public slots:
};

#endif // GRAPHICTHREAD_H
