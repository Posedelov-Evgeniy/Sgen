#include "soundpickerslist.h"
#include "ui_soundpickerslist.h"

SoundPickersList::SoundPickersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundPickersList)
{
    ui->setupUi(this);
}

SoundPickersList::~SoundPickersList()
{
    delete ui;
}

void SoundPickersList::addSoundPicker(QString file_name, QString function_name)
{
    int length = sounds.length();
    if (length<maxSounds)
    {
        QString t;
        SoundPicker *picker = new SoundPicker();
        t.sprintf("%08p", picker);
        picker->setObjectName("picker"+t);

        ui->sound_container->layout()->addWidget(picker);
        ui->sound_container->adjustSize();

        picker->setFilename(file_name);
        picker->setFunctionname(function_name);
        QObject::connect(picker, SIGNAL(add_new(SoundPicker*)), this, SLOT(add_sound(SoundPicker*)));
        QObject::connect(picker, SIGNAL(remove_item(SoundPicker*)), this, SLOT(remove_sound(SoundPicker*)));
        QObject::connect(picker, SIGNAL(changed()), this, SLOT(options_changing()));
        QObject::connect(picker, SIGNAL(add_new(SoundPicker*)), this, SLOT(options_changing()));
        QObject::connect(picker, SIGNAL(remove_item(SoundPicker*)), this, SLOT(options_changing()));
        sounds.append(picker);
        adjustSoundParams();
    }
}

void SoundPickersList::savePickersSettings(QSettings *settings)
{
    SoundPicker *picker;
    settings->setValue("sounds/sounds_count", sounds.length());
    int i = 1;
    foreach(picker, sounds) {
        settings->setValue("sounds/sound"+QString::number(i), picker->getFilename());
        settings->setValue("sounds/sound"+QString::number(i)+"_function", picker->getFunctionname());
        i++;
    }
}

void SoundPickersList::loadPickersSettings(QSettings *settings)
{
    SoundPicker *picker;
    foreach(picker, sounds) removeSoundPicker(picker);

    int length = settings->value("sounds/sounds_count", 1).toInt();
    if (length<1) length = 1;
    if (length>maxSounds) length = maxSounds;
    for (int i=1;i<=length;i++) {
        addSoundPicker(settings->value("sounds/sound"+QString::number(i), "").toString(), settings->value("sounds/sound"+QString::number(i)+"_function", "").toString());
    }
}

void SoundPickersList::setSNDOptions(SndController *sc)
{
    unsigned int ctag = sc->getBaseSoundList()->getTag() + 1;
    for(int i=0; i<sounds.length(); i++) {
        sc->getBaseSoundList()->setSound(i, sounds.at(i)->getFilename(), sounds.at(i)->getFunctionname(), ctag);
    }
    sc->getBaseSoundList()->setTag(ctag);
}

void SoundPickersList::removeSoundPicker(SoundPicker *p)
{
    int pos = sounds.indexOf(p);
    if (pos>=0) {
        ui->sound_container->layout()->removeWidget(p);
        sounds.at(pos)->deleteLater();
        delete sounds.at(pos);
        sounds.removeAt(pos);
        if (sounds.length()==0) {
            addSoundPicker("", "");
        } else {
            adjustSoundParams();
        }
    }
}

void SoundPickersList::adjustSoundParams()
{
    SoundPicker *picker;
    QList<QString> func_names;
    QString current_func_name;

    int i = 0;
    int j = 1;
    int last = sounds.length()-1;
    foreach(picker, sounds) {
        picker->setAddButtonEnabled(i==last);
        picker->setRemoveButtonEnabled(true);

        current_func_name = picker->getFunctionname();
        if (current_func_name.isEmpty() || current_func_name.indexOf(' ')>-1 || func_names.indexOf(current_func_name)>=0)
        {
            while (func_names.indexOf(current_func_name = "sound"+QString::number(j))>=0) j++;
            picker->setFunctionname(current_func_name);
        }
        func_names.append(current_func_name);

        i++;
    }
}

void SoundPickersList::add_sound(SoundPicker *p)
{
    addSoundPicker("", "");
}

void SoundPickersList::remove_sound(SoundPicker *p)
{
    removeSoundPicker(p);
}

void SoundPickersList::options_changing()
{
    emit pickers_options_changed();
}
