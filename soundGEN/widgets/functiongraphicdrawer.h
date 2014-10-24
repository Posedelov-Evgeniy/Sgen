#ifndef FUNCTIONGRAPHICDRAWER_H
#define FUNCTIONGRAPHICDRAWER_H

#include <QWidget>
#include "../sndcontroller.h"
#include "../classes/graphicthread.h"
#include "./mgraphicdrawsurface.h"
#include "./mfftdrawsurface.h"

namespace Ui {
class functionGraphicDrawer;
}

class functionGraphicDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit functionGraphicDrawer(QWidget *parent = 0);
    ~functionGraphicDrawer();

    void setGraphicFunction(GenSoundFunction value);

    void setT0(double value);
    void setAmp(double value);
    void setFreq(double value);

    int getDtIntValue() const;
    void setDtIntValue(int value);
    int getDtFftIntValue() const;
    void setDtFftIntValue(int value);
    int getKampIntValue() const;
    void setKampIntValue(int value);

    bool isGrouped();
    void setGrouped(bool value);

    bool isFft();
    void setFft(bool value);
private:
    Ui::functionGraphicDrawer *ui;
    static graphicThread *mThread;
    MGraphicDrawSurface *widget_drawer;
    MFftDrawSurface *widget_fft_drawer;
    bool block_change;
signals:
    void changed();
public slots:
    void run();
    void stop();
private slots:
    void drawCycle();
    void on_durationSlider_valueChanged(int value);
    void on_ampSlider_valueChanged(int value);
    void on_checkBox_grouped_stateChanged(int arg1);
    void on_checkBox_fft_stateChanged(int arg1);
    void on_durationSlider_fft_valueChanged(int value);
};

#endif // FUNCTIONGRAPHICDRAWER_H
