#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /* initial params */
    default_save_path = QCoreApplication::applicationDirPath()+"/config.cfg";
    default_functions_path = QCoreApplication::applicationDirPath()+"/config.cfg";
    auto_restart = false;
    close_on_stop = false;
    current_file_changed = false;
    ui->setupUi(this);
    base_title = windowTitle();

    /* adding widgets */
    functions_text = new UTextEdit();
    ui->functions_tab->layout()->addWidget(functions_text);

    /* setting options */
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Stop"));
    ui->buttonBox->button(QDialogButtonBox::Retry)->setText(tr("Restart"));
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    ui->buttonBox->setLayoutDirection(Qt::LeftToRight);
    #endif

    sound_stopped();
    sc = SndController::Instance();
    pickChannelsCount(2);

    /* loading settings */
    load_settings(default_save_path, true);

    /* creating signal-slot connections */
    QObject::connect(sc, SIGNAL(stopped()), this, SLOT(sound_stopped()));
    QObject::connect(sc, SIGNAL(started()), this, SLOT(sound_started()));
    QObject::connect(sc, SIGNAL(starting()), this, SLOT(sound_starting()));
    QObject::connect(sc, SIGNAL(write_message(QString)), this, SLOT(get_message(QString)));

    QObject::connect(functions_text, SIGNAL(textChangedC()), this, SLOT(options_changing()));
}

MainWindow::~MainWindow()
{
    functions_text->deleteLater();
    delete functions_text;
    delete ui;
}

void MainWindow::get_message(QString message)
{
    ui->statusBar->showMessage(message, 5000);
}

void MainWindow::sound_starting()
{
    emit fill_params();
    sc->setFunctionsStr(functions_text->document()->toPlainText());

    unsigned int ctag = sc->getBaseSoundList()->getTag() + 1;
    for(int i=0; i<sounds.length(); i++) {
        sc->getBaseSoundList()->setSound(i, sounds.at(i)->getFilename(), sounds.at(i)->getFunctionname(), ctag);
    }
    sc->getBaseSoundList()->setTag(ctag);
}

void MainWindow::sound_stopped()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(false);

    ui->action1_Mono->setEnabled(true);
    ui->action2_Stereo->setEnabled(true);
    ui->action4_Quadro->setEnabled(true);
    ui->action6->setEnabled(true);
    ui->action8->setEnabled(true);

    ui->actionOpen->setEnabled(true);

    if (auto_restart && !close_on_stop) {
        auto_restart = false;
        sc->run();
    } else {
        emit stop_channel_graphics();
    }

    if (close_on_stop) {
        this->close();
        return;
    }
}

void MainWindow::sound_started()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(true);

    ui->action1_Mono->setEnabled(false);
    ui->action2_Stereo->setEnabled(false);
    ui->action4_Quadro->setEnabled(false);
    ui->action6->setEnabled(false);
    ui->action8->setEnabled(false);

    ui->actionOpen->setEnabled(false);

    emit run_channel_graphics();
}

void MainWindow::on_MainWindow_destroyed()
{
    sc->stop();
}

void MainWindow::addSoundPicker(QString file_name, QString function_name)
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    sc->stop();

    if (!saveMessageBox()) {
        event->ignore();
        return;
    }

    save_settings(default_save_path);
    if (sc->running()) {
        close_on_stop = true;
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::setActionEnabled(int index, bool enabled)
{
    QList<QAction*> actionList = ui->menu->actions();
    if (!actionList.isEmpty() && actionList.length()>index) {
        actionList.at(index)->setEnabled(enabled);
    }
}

bool MainWindow::saveMessageBox()
{
    if (!current_file.isEmpty() && current_file_changed) {
        QMessageBox::StandardButton rbutton;
        rbutton = QMessageBox::question(this,tr("Save"),tr("Do you want to save current changes before close?"), QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel), QMessageBox::Cancel);
        if (rbutton == QMessageBox::Yes) {
            save_settings(current_file, false);
        } else if (rbutton == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::save_settings(QString filename, bool base_settings)
{
    QSettings settings(filename, QSettings::IniFormat);
    int i;

    settings.setValue("main/channels_count", sc->getChannelsCount());
    for(i=0; i<sc->getChannelsCount(); i++) {
        settings.setValue("main/function_"+QString::number(i), channels.at(i)->getFunction());
        settings.setValue("main/amp_"+QString::number(i), channels.at(i)->getAmp());
        settings.setValue("main/freq_"+QString::number(i), channels.at(i)->getFreq());
        settings.setValue("graphic/kamp_"+QString::number(i), channels.at(i)->getDrawer()->getKampIntValue());
        settings.setValue("graphic/dt_"+QString::number(i), channels.at(i)->getDrawer()->getDtIntValue());
    }

    SoundPicker *picker;
    settings.setValue("sounds/sounds_count", sounds.length());
    i = 1;
    foreach(picker, sounds) {
        settings.setValue("sounds/sound"+QString::number(i), picker->getFilename());
        settings.setValue("sounds/sound"+QString::number(i)+"_function", picker->getFunctionname());
        i++;
    }

    if (base_settings) {
        QRect gg = this->geometry();
        settings.setValue("window/left", gg.left());
        settings.setValue("window/top", gg.top());
        settings.setValue("window/width", gg.width());
        settings.setValue("window/height", gg.height());

        settings.setValue("main/opened_file", current_file);
        settings.setValue("main/opened_file_changed", current_file_changed);

        QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp.cfg");
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << functions_text->document()->toPlainText();
            file.close();
            if (stream.status() != QTextStream::Ok)
            {
                qDebug() << tr("Error writing functions.cpp.cfg");
            }
        }
    } else {
        settings.setValue("main/user_functions", functions_text->document()->toPlainText());
        current_file = filename;
        current_file_changed = false;
        setWindowTitle(base_title+": "+filename);
        setActionEnabled(4, true);
    }
}

void MainWindow::load_settings(QString filename, bool base_settings)
{
    QSettings settings(filename, QSettings::IniFormat);
    int i;

    SoundPicker *picker;
    foreach(picker, sounds) removeSoundPicker(picker);

    int length = settings.value("sounds/sounds_count", 1).toInt();
    if (length<1) length = 1;
    if (length>maxSounds) length = maxSounds;
    for (i=1;i<=length;i++) {
        addSoundPicker(settings.value("sounds/sound"+QString::number(i), "").toString(), settings.value("sounds/sound"+QString::number(i)+"_function", "").toString());
    }

    int channels_cnt = settings.value("main/channels_count", 2).toInt();
    if (channels_cnt<=0 || channels_cnt>8) channels_cnt=2;
    pickChannelsCount(channels_cnt);

    for(i=0; i<sc->getChannelsCount(); i++) {
        channels.at(i)->setFunction(settings.value("main/function_"+QString::number(i), "sin(k*t)").toString());
        channels.at(i)->setAmp(settings.value("main/amp_"+QString::number(i), 1).toDouble());
        channels.at(i)->setFreq(settings.value("main/freq_"+QString::number(i), 500).toDouble());
        channels.at(i)->getDrawer()->setKampIntValue(settings.value("graphic/kamp_"+QString::number(i), 0).toDouble());
        channels.at(i)->getDrawer()->setDtIntValue(settings.value("graphic/dt_"+QString::number(i), 300).toDouble());
    }

    if (base_settings) {
        QWidget::move(settings.value("window/left", 100).toInt(), settings.value("window/top", 100).toInt());
        QWidget::resize(settings.value("window/width", 400).toInt(), settings.value("window/height", 300).toInt());

        QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp.cfg");
        if(!file.exists()){
            qDebug() << tr("Functions.cpp.cfg not exists");
        }
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            QString funct_str = stream.readAll().toUtf8();
            functions_text->document()->setPlainText(funct_str);
        }

        current_file = settings.value("main/opened_file", "").toString();
        if (!QFile::exists(current_file)) {
            current_file = "";
        } else {
            current_file_changed = settings.value("main/opened_file_changed", false).toBool();
        }
    } else {
        functions_text->document()->setPlainText(settings.value("main/user_functions", "").toString());
        current_file = filename;
        current_file_changed = false;
    }

    if (!current_file.isEmpty()) {
        setWindowTitle(base_title+": "+current_file);
        setActionEnabled(4, true);
    } else {
        this->setWindowTitle(base_title);
        setActionEnabled(4, false);
    }
}

void MainWindow::removeSoundPicker(SoundPicker *p)
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

void MainWindow::adjustSoundParams()
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

void MainWindow::add_sound(SoundPicker *p)
{
    addSoundPicker("", "");
}

void MainWindow::remove_sound(SoundPicker *p)
{
    removeSoundPicker(p);
}

void MainWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        sc->run();
    } else if(button == ui->buttonBox->button(QDialogButtonBox::Retry)) {
        auto_restart = true;
        sc->stop();
    } else {
        sc->stop();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (!saveMessageBox()) return;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "./examples", tr("SNDGEN options file (*.sndgopt)"));
    if (!fileName.isEmpty()) {
        load_settings(fileName, false);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (current_file.isEmpty() || !QFile::exists(current_file)) {
        on_actionSave_as_triggered();
    } else {
        save_settings(current_file, false);
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "./examples", tr("SNDGEN options file (*.sndgopt)"));
    if (!fileName.isEmpty()) {
        if (fileName.indexOf(".sndgopt")<0) fileName = fileName + ".sndgopt";
        save_settings(fileName, false);
    }
}


void MainWindow::on_actionClose_triggered()
{
    if (!saveMessageBox()) return;
    current_file = "";
    current_file_changed = false;
    setActionEnabled(4, false);
    this->setWindowTitle(base_title);
}

void MainWindow::options_changing()
{
    current_file_changed = true;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::setChannelsCount(unsigned int count)
{
    if (count<channels.length()) {
        while (count>=0 && count<channels.length()) {
            ui->verticalLayout_channels->removeWidget(channels.last());
            channels.last()->deleteLater();
            delete channels.last();
            channels.removeLast();
        }
    } else if (count>channels.length()) {
        while (count>channels.length()) {
            ChannelSettings *chset = new ChannelSettings(0, channels.length());
            ui->verticalLayout_channels->addWidget(chset);
            QObject::connect(this, SIGNAL(fill_params()), chset, SLOT(init_snd_channel_params()));
            QObject::connect(this, SIGNAL(run_channel_graphics()), chset, SLOT(run_graphic()));
            QObject::connect(this, SIGNAL(stop_channel_graphics()), chset, SLOT(stop_graphic()));
            QObject::connect(chset, SIGNAL(options_changed()), this, SLOT(options_changing()));
            channels.append(chset);
        }
    }

    sc->setChannelsCount(count);
}

void MainWindow::pickChannelsCount(unsigned int count)
{
    ui->action1_Mono->setChecked(count==1);
    ui->action2_Stereo->setChecked(count==2);
    ui->action4_Quadro->setChecked(count==4);
    ui->action6->setChecked(count==6);
    ui->action8->setChecked(count==8);
    setChannelsCount(count);
}

void MainWindow::on_action1_Mono_triggered()
{
    pickChannelsCount(1);
}

void MainWindow::on_action2_Stereo_triggered()
{
    pickChannelsCount(2);
}

void MainWindow::on_action4_Quadro_triggered()
{
    pickChannelsCount(4);
}

void MainWindow::on_action6_triggered()
{
    pickChannelsCount(6);
}

void MainWindow::on_action8_triggered()
{
    pickChannelsCount(8);
}
