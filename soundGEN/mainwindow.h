#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include "sndcontroller.h"
#include "widgets/soundpicker.h"
#include "widgets/functiongraphicdrawer.h"
#include "widgets/dialogfunctions.h"
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

private slots:

    void sound_stopped();

    void sound_started();

    void sound_starting();

    void cycle_starting();

    void add_sound(SoundPicker* p);

    void remove_sound(SoundPicker* p);

    void get_message(QString message);

    void paste_function_accepted();

    void on_MainWindow_destroyed();

    void on_doubleSpinBox_amp_left_valueChanged(double arg1);

    void on_doubleSpinBox_amp_right_valueChanged(double arg1);

    void on_doubleSpinBox_freq_left_valueChanged(double arg1);

    void on_doubleSpinBox_freq_right_valueChanged(double arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_left_dialog_functions_btn_clicked();

    void on_right_dialog_functions_btn_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionExit_triggered();

    void on_actionClose_triggered();

    void options_changing();

private:
    static const int maxSounds = 10;
    Ui::MainWindow *ui;
    DialogFunctions *dialog_functions;
    SndController *sc;
    bool auto_restart, close_on_stop, current_file_changed;
    QString default_save_path, default_functions_path;
    QString base_title;
    QString current_file;
    QList<SoundPicker*> sounds;
    functionGraphicDrawer *left_drawer;
    functionGraphicDrawer *right_drawer;
    UTextEdit *functions_text;
    UTextEdit *left_function;
    UTextEdit *right_function;
    UTextEdit *dialog_for_edit;

    void removeSoundPicker(SoundPicker* p);
    void adjustSoundParams();
    void addSoundPicker(QString file_name, QString function_name);
    void closeEvent(QCloseEvent *event);
    void setActionEnabled(int index, bool enabled = false);
    bool saveMessageBox();

    void save_settings(QString filename, bool base_settings = true);
    void load_settings(QString filename, bool base_settings = true);
};

#endif // MAINWINDOW_H
