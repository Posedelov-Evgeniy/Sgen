#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include "sndcontroller.h"
#include "widgets/soundpickerslist.h"
#include "widgets/variablepickerslist.h"
#include "widgets/channelsettings.h"
#include "widgets/dialogexport.h"
#include "classes/utextedit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void fill_params();
    void run_channel_graphics();
    void stop_channel_graphics();

private slots:

    void sound_stopped();
    void sound_started();
    void options_changing();
    void get_message(QString message);

    void on_MainWindow_destroyed();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionExit_triggered();
    void on_actionClose_triggered();
    void channel_options_changing(int channel_index);
    void on_action1_Mono_triggered();
    void on_action2_Stereo_triggered();
    void on_action4_Quadro_triggered();
    void on_action6_triggered();
    void on_action8_triggered();
    void on_actionExport_to_triggered();
    void on_action25_ms_triggered();
    void on_action50_ms_triggered();
    void on_action250_ms_triggered();
    void on_action1000_ms_triggered();
    void on_action10000_ms_triggered();
    void on_actionChannel_graphics_triggered();
    void on_actionChannel_instant_frequency_triggered();
    void on_actionChannel_instant_amplitude_triggered();
    void on_actionResult_linear_transition_triggered();
    void on_actionResult_square_transition_triggered();
    void on_actionVariables_linear_transition_triggered();
    void on_actionVariables_square_transition_triggered();
    void on_actionNo_transition_triggered();
    void on_action0_5s_triggered();
    void on_action1s_triggered();
    void on_action2s_triggered();
    void on_action5s_triggered();
    void on_action10s_triggered();
    void on_action30s_triggered();

private:
    Ui::MainWindow *ui;
    SndController *sc;
    bool auto_restart, close_on_stop, current_file_changed;
    QString default_save_path, default_functions_path;
    QString base_title;
    QString current_file;

    QList<ChannelSettings*> channels;
    UTextEdit *functions_text;
    UTextEdit *dialog_for_edit;
    DialogExport *export_form;
    SoundPickersList *pickers_list;
    VariablePickersList *variables_list;

    void closeEvent(QCloseEvent *event);
    void setActionEnabled(int index, bool enabled = false);
    bool saveMessageBox();

    void save_settings(QString filename, bool base_settings = true);
    void load_settings(QString filename, bool base_settings = true);
    void pickBufferSize(unsigned int size);
    void pickChannelsCount(unsigned int count);
    void setChannelsCount(unsigned int count);
    void doSetParams();
    void doSetVisibility();
    void pickTransitionTime(double timelen);
    void pickTransitionType(SignalControllerVariablesTransition ttype);
};

#endif // MAINWINDOW_H
