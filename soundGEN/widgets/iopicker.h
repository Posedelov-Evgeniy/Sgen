#ifndef IOPICKER_H
#define IOPICKER_H

#include <QComboBox>
#include "../sndcontroller.h"

namespace Ui {
class IOPicker;
}

struct InputDeviceInfo {
    bool active;
    int device_id;
    QString device_guid;
    unsigned recording_buffer_size;
    unsigned latency;
    qint16 drift;
    QString function_name;
};
typedef QMap<int, InputDeviceInfo> CInputs;

class IOPicker : public QWidget
{
    Q_OBJECT

public:
    explicit IOPicker(QWidget *parent = 0);
    ~IOPicker();

    void savePickersSettings(QSettings *settings);
    void loadPickersSettings(QSettings *settings);

    void setOutChannelsCount(unsigned int new_channels_count);
    void setOutBuffer(unsigned int new_buffer_size);

    int getOutIndex();
signals:
    void channels_count_changed(unsigned int new_channels_count);
    void play_buffer_changed(unsigned int new_buffer_size);

private slots:
    void on_spinBox_play_buffer_valueChanged(int arg1);
    void on_spinBox_channels_valueChanged(int arg1);
    void on_toolButton_refresh_clicked();
    void on_toolButton_in_refresh_clicked();
    void on_toolButton_in_refresh_2_clicked();
    void on_checkBox_in_active_toggled(bool checked);
    void on_checkBox_in_active_2_toggled(bool checked);

    void on_comboBox_soundcard_currentIndexChanged(int index);

private:
    Ui::IOPicker *ui;
    int out_index;
    QString out_guid;
    CInputs *inputs;

    void playing_devices(QComboBox *combobox, QString sel_guid = "");
    void recording_devices(QComboBox *combobox, QString sel_guid = "", int in_number = 0);
};

#endif // IOPICKER_H
