#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void on_functionsButton_clicked();

    void on_pushButton_clicked();
    
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_3_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_doubleSpinBox_4_valueChanged(double arg1);

    void on_MainWindow_destroyed();

private:
    Ui::MainWindow *ui;
    SndController *sc;
    bool auto_restart;
};

#endif // MAINWINDOW_H
