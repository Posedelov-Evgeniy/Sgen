#ifndef IOPICKER_H
#define IOPICKER_H

#include <QComboBox>
#include "../sndcontroller.h"

namespace Ui {
class IOPicker;
}

class IOPicker : public QWidget
{
    Q_OBJECT

public:
    explicit IOPicker(QWidget *parent = 0);
    ~IOPicker();

private slots:
    void on_spinBox_play_buffer_valueChanged(int arg1);
    void on_spinBox_channels_valueChanged(int arg1);
    void on_toolButton_refresh_clicked();
    void on_toolButton_in_refresh_clicked();
    void on_toolButton_in_refresh_2_clicked();

private:
    Ui::IOPicker *ui;
    void playing_devices(QComboBox *combobox, QString sel_guid = "");
    void recording_devices(QComboBox *combobox, QString sel_guid = "");
};

#endif // IOPICKER_H
