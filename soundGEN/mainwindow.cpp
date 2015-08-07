#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /* initial params */
    default_save_path = EnvironmentInfo::getConfigsPath()+"/config.cfg";
    default_functions_path = EnvironmentInfo::getConfigsPath()+"/config.cfg";
    auto_restart = false;
    close_on_stop = false;
    current_file_changed = false;
    ui->setupUi(this);
    base_title = windowTitle();

    /* adding widgets */
    pickers_list = new SoundPickersList(ui->sound_container);
    variables_list = new VariablePickersList(ui->variables_container);
    export_form = new DialogExport(parent);
    functions_text = new UTextEdit();
    ui->functions_tab->layout()->addWidget(functions_text);
    ui->sound_container->layout()->addWidget(pickers_list);
    ui->variables_container->layout()->addWidget(variables_list);

    /* setting options */
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Stop"));
    ui->buttonBox->button(QDialogButtonBox::Retry)->setText(tr("Restart"));
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__ANDROID__)
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
    QObject::connect(sc, SIGNAL(write_message(QString)), this, SLOT(get_message(QString)));

    QObject::connect(functions_text, SIGNAL(textChangedC()), this, SLOT(options_changing()));
    QObject::connect(pickers_list, SIGNAL(pickers_options_changed()), this, SLOT(options_changing()));
    QObject::connect(variables_list, SIGNAL(variables_options_changed()), this, SLOT(options_changing()));
}

MainWindow::~MainWindow()
{
    functions_text->deleteLater();
    delete functions_text;
    delete export_form;
    delete pickers_list;
    delete variables_list;
    delete ui;
}

void MainWindow::get_message(QString message)
{
    ui->statusBar->showMessage(message, 5000);
}

void MainWindow::doSetParams()
{
    emit fill_params();
    sc->setFunctionsStr(functions_text->document()->toPlainText());

    pickers_list->setSNDOptions(sc);
    variables_list->setSNDOptions(sc);
}

void MainWindow::sound_stopped()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(false);

    ui->actionExport_to->setEnabled(true);
    ui->action1_Mono->setEnabled(true);
    ui->action2_Stereo->setEnabled(true);
    ui->action4_Quadro->setEnabled(true);
    ui->action6->setEnabled(true);
    ui->action8->setEnabled(true);

    ui->actionOpen->setEnabled(true);
    emit stop_channel_graphics();

    if (auto_restart && !close_on_stop) {
        auto_restart = false;
        doSetParams();
        sc->run();
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

    ui->actionExport_to->setEnabled(false);
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (sc->running()) {
        sc->stop();
    }

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
        settings.setValue("graphic/group_"+QString::number(i), channels.at(i)->getDrawer()->isGrouped());
        settings.setValue("graphic/fft_"+QString::number(i), channels.at(i)->getDrawer()->isFft());
        settings.setValue("graphic/dt_fft_"+QString::number(i), channels.at(i)->getDrawer()->getDtFftIntValue());
    }

    pickers_list->savePickersSettings(&settings);
    variables_list->savePickersSettings(&settings);

    if (base_settings) {
        QRect gg = this->geometry();
        settings.setValue("window/left", gg.left());
        settings.setValue("window/top", gg.top());
        settings.setValue("window/width", gg.width());
        settings.setValue("window/height", gg.height());

        settings.setValue("main/opened_file", current_file);
        settings.setValue("main/opened_file_changed", current_file_changed);

        QFile file(EnvironmentInfo::getConfigsPath()+"/functions.cpp.cfg");
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

    pickers_list->loadPickersSettings(&settings);
    variables_list->loadPickersSettings(&settings);

    int channels_cnt = settings.value("main/channels_count", 2).toInt();
    if (channels_cnt<=0 || channels_cnt>8) channels_cnt=2;
    pickChannelsCount(channels_cnt);

    for(i=0; i<sc->getChannelsCount(); i++) {
        channels.at(i)->setFunction(settings.value("main/function_"+QString::number(i), "sin(k*t)").toString());
        channels.at(i)->setAmp(settings.value("main/amp_"+QString::number(i), 1).toDouble());
        channels.at(i)->setFreq(settings.value("main/freq_"+QString::number(i), 500).toDouble());
        channels.at(i)->getDrawer()->setKampIntValue(settings.value("graphic/kamp_"+QString::number(i), 0).toDouble());
        channels.at(i)->getDrawer()->setDtIntValue(settings.value("graphic/dt_"+QString::number(i), 300).toDouble());
        channels.at(i)->getDrawer()->setGrouped(settings.value("graphic/group_"+QString::number(i), true).toBool());
        channels.at(i)->getDrawer()->setFft(settings.value("graphic/fft_"+QString::number(i), false).toBool());
        channels.at(i)->getDrawer()->setDtFftIntValue(settings.value("graphic/dt_fft_"+QString::number(i), 100).toDouble());
    }

    if (base_settings) {
        QWidget::move(settings.value("window/left", 100).toInt(), settings.value("window/top", 100).toInt());
        QWidget::resize(settings.value("window/width", 400).toInt(), settings.value("window/height", 300).toInt());

        QFile file(EnvironmentInfo::getConfigsPath()+"/functions.cpp.cfg");
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

void MainWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        doSetParams();
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

void MainWindow::channel_options_changing(int channel_index)
{
    options_changing();
    if (channel_index>=0 && channel_index<channels.length() && channels.at(channel_index)->getDrawer()->isGrouped()) {
        int dt = channels.at(channel_index)->getDrawer()->getDtIntValue();
        int kamp = channels.at(channel_index)->getDrawer()->getKampIntValue();
        bool is_fft = channels.at(channel_index)->getDrawer()->isFft();
        int fft_dt = channels.at(channel_index)->getDrawer()->getDtFftIntValue();
        for(int i = 0; i<channels.length(); i++) {
            if (i!=channel_index && channels.at(i)->getDrawer()->isGrouped()) {
                channels.at(i)->getDrawer()->setDtIntValue(dt);
                channels.at(i)->getDrawer()->setKampIntValue(kamp);
                channels.at(i)->getDrawer()->setDtFftIntValue(fft_dt);
                channels.at(i)->getDrawer()->setFft(is_fft);
            }
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::setChannelsCount(unsigned int count)
{
    if (count<channels.length()) {
        while (count>=0 && count<channels.length()) {
            ui->channels_container->layout()->removeWidget(channels.last());
            channels.last()->deleteLater();
            delete channels.last();
            channels.removeLast();
        }
    } else if (count>channels.length()) {
        while (count>channels.length()) {
            ChannelSettings *chset = new ChannelSettings(0, channels.length(), count);
            ui->channels_container->layout()->addWidget(chset);
            QObject::connect(this, SIGNAL(fill_params()), chset, SLOT(init_snd_channel_params()));
            QObject::connect(this, SIGNAL(run_channel_graphics()), chset, SLOT(run_graphic()));
            QObject::connect(this, SIGNAL(stop_channel_graphics()), chset, SLOT(stop_graphic()));
            QObject::connect(chset, SIGNAL(options_changed(int)), this, SLOT(channel_options_changing(int)));
            channels.append(chset);
        }
    }
    for(unsigned int i=0; i<count; i++) channels.at(i)->setChannelsCount(count);

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

void MainWindow::on_actionExport_to_triggered()
{
    doSetParams();
    export_form->exec();
}
