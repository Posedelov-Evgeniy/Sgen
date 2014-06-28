#ifndef SOUNDPICKER_H
#define SOUNDPICKER_H

#include <QWidget>
#include <QFileDialog>

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
    void setFilename(QString filename);
    void setFunctionname(QString functionname);
    QString getFilename();
    QString getFunctionname();
signals:
    void remove_item(SoundPicker *p);
    void add_new(SoundPicker *p);

private slots:
    void on_pushButton_sound_clicked();

    void on_pushButton_sound_remove_clicked();

    void on_pushButton_sound_add_clicked();

private:
    Ui::SoundPicker *ui;
    int self_index;
};

#endif // SOUNDPICKER_H
