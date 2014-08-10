#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <QWidget>
#include "../classes/utextedit.h"
#include "dialogfunctions.h"
#include "functiongraphicdrawer.h"

namespace Ui {
class ChannelSettings;
}

class ChannelSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ChannelSettings(QWidget *parent = 0, unsigned int base_channel_index = 0);
    ~ChannelSettings();

    QString getFunction();
    void setFunction(QString ftext);
    double getAmp();
    void setAmp(double amp);
    double getFreq();
    void setFreq(double freq);

    functionGraphicDrawer *getDrawer();

    void setVisibleAmp(double amp);
    void setVisibleFreq(double freq);
signals:
    void options_changed();
public slots:
    void cycle_starting();
    void options_changing();
    void init_snd_channel_params();
    void run_graphic();
    void stop_graphic();
private slots:
    void on_doubleSpinBox_amp_valueChanged(double arg1);

    void on_doubleSpinBox_freq_valueChanged(double arg1);

    void on_dialog_functions_btn_clicked();

    void paste_function_accepted();
private:
    Ui::ChannelSettings *ui;
    DialogFunctions *dialog_functions;
    UTextEdit *function_edit;
    SndController *sc;
    unsigned int channel_index;
    functionGraphicDrawer *channel_drawer;
};

#endif // CHANNELSETTINGS_H
