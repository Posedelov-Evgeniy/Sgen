#ifndef SOUNDPICKERSLIST_H
#define SOUNDPICKERSLIST_H

#include <QWidget>
#include "soundpicker.h"

namespace Ui {
class SoundPickersList;
}

class SoundPickersList : public QWidget
{
    Q_OBJECT

public:
    explicit SoundPickersList(QWidget *parent = 0);
    ~SoundPickersList();

    void savePickersSettings(QSettings *settings);
    void loadPickersSettings(QSettings *settings);
    void setSNDOptions(SndController* sc);

signals:
    void pickers_options_changed();

private slots:
    void add_sound(SoundPicker* p);
    void remove_sound(SoundPicker* p);
    void options_changing();

private:
    Ui::SoundPickersList *ui;
    static const int maxSounds = 10;
    QList<SoundPicker*> sounds;

    void addSoundPicker(QString file_name, QString function_name);
    void removeSoundPicker(SoundPicker* p);
    void adjustParams();
};

#endif // SOUNDPICKERSLIST_H
