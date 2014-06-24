#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include "sndcontroller.h"

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

    void get_message(QString message);

    void on_functionsButton_clicked();

    void on_MainWindow_destroyed();

    void on_pushButton_sound1_clicked();

    void on_pushButton_play_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_restart_clicked();

    void on_doubleSpinBox_amp_left_valueChanged(double arg1);

    void on_doubleSpinBox_amp_right_valueChanged(double arg1);

    void on_doubleSpinBox_freq_left_valueChanged(double arg1);

    void on_doubleSpinBox_freq_right_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    SndController *sc;
    bool auto_restart;
    bool panel_opened;
    int base_width;
};

#endif // MAINWINDOW_H
