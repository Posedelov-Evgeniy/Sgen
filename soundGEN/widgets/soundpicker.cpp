#include "soundpicker.h"
#include "ui_soundpicker.h"

SoundPicker::SoundPicker(QWidget *parent, bool add_button, bool remove_button) :
    QWidget(parent),
    ui(new Ui::SoundPicker)
{
    ui->setupUi(this);
    ui->pushButton_sound_add->setEnabled(add_button);
    ui->pushButton_sound_remove->setEnabled(remove_button);
}

SoundPicker::~SoundPicker()
{
    delete ui;
}

void SoundPicker::setAddButtonEnabled(bool enabled)
{
    ui->pushButton_sound_add->setEnabled(enabled);
}

void SoundPicker::setRemoveButtonEnabled(bool enabled)
{
    ui->pushButton_sound_remove->setEnabled(enabled);
}

void SoundPicker::setFilename(QString filename)
{
    ui->lineEdit_sound->setText(filename);
}

void SoundPicker::setFunctionname(QString functionname)
{
    ui->lineEdit_function->setText(functionname);
}

QString SoundPicker::getFilename()
{
    return ui->lineEdit_sound->text();
}

QString SoundPicker::getFunctionname()
{
    return ui->lineEdit_function->text();
}

void SoundPicker::on_pushButton_sound_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Sound files (*.mp3 *.wav *.ogg)"));
    setFilename(fileName);
}

void SoundPicker::on_pushButton_sound_remove_clicked()
{
    emit remove_item(this);
}

void SoundPicker::on_pushButton_sound_add_clicked()
{
    emit add_new(this);
}
