#ifndef SOUNDPICKER_H
#define SOUNDPICKER_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "../sndcontroller.h"

namespace Ui {
class SoundPicker;
}

class SoundPicker : public QWidget
{
    Q_OBJECT

public:
    explicit SoundPicker(QWidget *parent = 0, bool add_button = true, bool remove_button = true);
    ~SoundPicker();
    void setAddButtonEnabled(bool enabled);
    void setRemoveButtonEnabled(bool enabled);
    void setFilename(QString filename, bool show_warnings = false);
    void setFunctionname(QString functionname);
    QString getFilename();
    QString getFunctionname();
signals:
    void remove_item(SoundPicker *p);
    void add_new(SoundPicker *p);
    void changed();
private slots:
    void on_pushButton_sound_clicked();

    void on_pushButton_sound_remove_clicked();

    void on_pushButton_sound_add_clicked();

    void params_changed();

private:
    Ui::SoundPicker *ui;
    int self_index;

    void checkSound(bool show_warnings);
};

#endif // SOUNDPICKER_H
