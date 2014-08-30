#include "soundpicker.h"
#include "ui_soundpicker.h"

SoundPicker::SoundPicker(QWidget *parent, bool add_button, bool remove_button) :
    QWidget(parent),
    ui(new Ui::SoundPicker)
{
    ui->setupUi(this);
    ui->label_channels->setVisible(false);
    ui->pushButton_sound_add->setEnabled(add_button);
    ui->pushButton_sound_remove->setEnabled(remove_button);
    QObject::connect(ui->lineEdit_sound, SIGNAL(textChanged(QString)), this, SLOT(params_changed()));
    QObject::connect(ui->lineEdit_function, SIGNAL(textChanged(QString)), this, SLOT(params_changed()));
    checkSound(false);
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

void SoundPicker::setFilename(QString filename, bool show_warnings)
{
    ui->lineEdit_sound->setText(filename);
    checkSound(show_warnings);
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
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    QString bfile = "";
    #else
    QString bfile = "/home";
    #endif
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), bfile, tr("Sound files (*.mp3 *.wav *.ogg)"));
    setFilename(fileName, true);
}

void SoundPicker::on_pushButton_sound_remove_clicked()
{
    emit remove_item(this);
}

void SoundPicker::on_pushButton_sound_add_clicked()
{
    emit add_new(this);
}

void SoundPicker::params_changed()
{
    emit changed();
}

void SoundPicker::checkSound(bool show_warnings)
{
    ui->label_length->setText(tr("Not loaded"));
    ui->label_channels->setText("");
    ui->label_channels->setVisible(false);
    if (getFilename().isEmpty()) return;
    FMOD::System *system = SndController::Instance()->getFmodSystem();
    bool result = false;

    if (system)
    {
        FMOD::Sound      *sound;
        if (system->createSound(qPrintable(getFilename()), FMOD_HARDWARE, 0, &sound) == FMOD_OK)
        {
            if (sound)
            {
                unsigned int length;
                FMOD_SOUND_TYPE stype;
                FMOD_SOUND_FORMAT sformat;
                int channels_count = 0;
                int bits_count;
                sound->getFormat(&stype, &sformat, &channels_count, &bits_count);

                if (sound->getLength(&length, FMOD_TIMEUNIT_MS) == FMOD_OK) {
                    ui->label_length->setText(tr("Length: ") + QString::number(length/1000.0)+ tr("s"));
                    ui->label_channels->setText(tr("Channels: ") + QString::number(channels_count));
                    ui->label_channels->setVisible(true);
                    result = true;
                }
            }
            sound->release();
        }
    }

    if (!result && show_warnings) {
        QMessageBox::warning(this,tr("Incorrect file"),tr("File \"%filename%\" is incorrect or not exists.").replace("%filename%",getFilename()), QMessageBox::StandardButtons(QMessageBox::Ok), QMessageBox::Ok);
    }
}

