#ifndef FUNCTIONGRAPHICDRAWER_H
#define FUNCTIONGRAPHICDRAWER_H

#include <QWidget>
#include "sndcontroller.h"
#include "widgets/graphicthread.h"
#include "widgets/mgraphicdrawsurface.h"

namespace Ui {
class functionGraphicDrawer;
}

class functionGraphicDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit functionGraphicDrawer(QWidget *parent = 0);
    ~functionGraphicDrawer();

    void setGraphicFunction(const GenSoundFunction &value);

    double getT0() const;
    void setT0(double value);

    double getAmp() const;
    void setAmp(double value);

    double getFreq() const;
    void setFreq(double value);

private:
    Ui::functionGraphicDrawer *ui;
    static graphicThread *mThread;
    MGraphicDrawSurface *widget_drawer;
public slots:
    void run();
    void stop();
private slots:
    void drawCycle();
    void on_durationSlider_valueChanged(int value);
    void on_ampSlider_valueChanged(int value);
};

#endif // FUNCTIONGRAPHICDRAWER_H
