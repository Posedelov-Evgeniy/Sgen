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

    ui->action25_ms->setEnabled(true);
    ui->action50_ms->setEnabled(true);
    ui->action250_ms->setEnabled(true);
    ui->action1000_ms->setEnabled(true);
    ui->action10000_ms->setEnabled(true);

    ui->actionNo_transition->setEnabled(true);
    ui->actionResult_linear_transition->setEnabled(true);
    ui->actionResult_square_transition->setEnabled(true);
    ui->actionVariables_linear_transition->setEnabled(true);
    ui->actionVariables_square_transition->setEnabled(true);
    ui->menuTransition_time->setEnabled(true);

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

    ui->action25_ms->setEnabled(false);
    ui->action50_ms->setEnabled(false);
    ui->action250_ms->setEnabled(false);
    ui->action1000_ms->setEnabled(false);
    ui->action10000_ms->setEnabled(false);

    ui->actionNo_transition->setEnabled(false);
    ui->actionResult_linear_transition->setEnabled(false);
    ui->actionResult_square_transition->setEnabled(false);
    ui->actionVariables_linear_transition->setEnabled(false);
    ui->actionVariables_square_transition->setEnabled(false);
    ui->menuTransition_time->setEnabled(false);

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
    settings.setValue("main/transition_type", sc->getTransitionType());
    settings.setValue("main/transition_time", sc->getTransitionTime());
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
    settings.setValue("main/graphics_visible", ui->actionChannel_graphics->isChecked());
    settings.setValue("main/instamp_visible", ui->actionChannel_instant_amplitude->isChecked());
    settings.setValue("main/instfreq_visible", ui->actionChannel_instant_frequency->isChecked());

    pickers_list->savePickersSettings(&settings);
    variables_list->savePickersSettings(&settings);

    settings.setValue("main/buffer_size", sc->getSystemBufferMsSize());

    if (base_settings) {

        settings.setValue("window/maximized", isMaximized());
        settings.setValue("window/minimized", isMinimized());
        if (!isMaximized() && !isMinimized()) {
            QRect gg = this->geometry();
            settings.setValue("window/left", gg.left());
            settings.setValue("window/top", gg.top());
            settings.setValue("window/width", gg.width());
            settings.setValue("window/height", gg.height());
        }

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

    ui->actionChannel_graphics->setChecked(settings.value("main/graphics_visible", ui->actionChannel_graphics->isChecked()).toBool());
    ui->actionChannel_instant_amplitude->setChecked(settings.value("main/instamp_visible", ui->actionChannel_instant_amplitude->isChecked()).toBool());
    ui->actionChannel_instant_frequency->setChecked(settings.value("main/instfreq_visible", ui->actionChannel_instant_frequency->isChecked()).toBool());

    pickTransitionType((SignalControllerVariablesTransition) settings.value("main/transition_type", 1).toInt());
    pickTransitionTime(settings.value("main/transition_time", 1).toDouble());

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
    doSetVisibility();

    pickBufferSize(settings.value("main/buffer_size", 1000).toInt());

    if (base_settings) {
        QWidget::move(settings.value("window/left", 100).toInt(), settings.value("window/top", 100).toInt());
        QWidget::resize(settings.value("window/width", 400).toInt(), settings.value("window/height", 300).toInt());

        if (settings.value("window/maximized", false).toBool()) {
            showMaximized();
        }
        if (settings.value("window/minimized", false).toBool()) {
            showMinimized();
        }

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

void MainWindow::pickBufferSize(unsigned int size)
{
    switch(size) {
        case 25:
        case 50:
        case 250:
        case 1000:
        case 10000:
            break;
        default:
            size = 1000;
    }

    ui->action25_ms->setChecked(size==25);
    ui->action50_ms->setChecked(size==50);
    ui->action250_ms->setChecked(size==250);
    ui->action1000_ms->setChecked(size==1000);
    ui->action10000_ms->setChecked(size==10000);
    sc->setSystemBufferMsSize(size);
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

void MainWindow::on_action25_ms_triggered()
{
    pickBufferSize(25);
}

void MainWindow::on_action50_ms_triggered()
{
    pickBufferSize(50);
}

void MainWindow::on_action250_ms_triggered()
{
    pickBufferSize(250);
}

void MainWindow::on_action1000_ms_triggered()
{
    pickBufferSize(1000);
}

void MainWindow::on_action10000_ms_triggered()
{
    pickBufferSize(10000);
}

void MainWindow::doSetVisibility()
{
    for(int i = 0; i<channels.length(); i++) {
        channels.at(i)->setGraphicsVisibility(ui->actionChannel_graphics->isChecked());
        channels.at(i)->setInstAmpVisibility(ui->actionChannel_instant_amplitude->isChecked());
        channels.at(i)->setInstFreqVisibility(ui->actionChannel_instant_frequency->isChecked());
    }
    sc->setPlayAnalizeActivity(ui->actionChannel_instant_amplitude->isChecked() || ui->actionChannel_instant_frequency->isChecked());
}

void MainWindow::on_actionChannel_graphics_triggered()
{
    doSetVisibility();
}

void MainWindow::on_actionChannel_instant_frequency_triggered()
{
    doSetVisibility();
}

void MainWindow::on_actionChannel_instant_amplitude_triggered()
{
    doSetVisibility();
}

void MainWindow::pickTransitionType(SignalControllerVariablesTransition ttype)
{
    ui->actionResult_linear_transition->setChecked(ttype==SCVT_LinearBuff);
    ui->actionResult_square_transition->setChecked(ttype==SCVT_SquareBuff);
    ui->actionVariables_linear_transition->setChecked(ttype==SCVT_LinearVar);
    ui->actionVariables_square_transition->setChecked(ttype==SCVT_SquareVar);
    ui->actionNo_transition->setChecked(ttype==SCVT_None);
    sc->setTransitionType(ttype);
    ui->menuTransition_time->setEnabled(ttype!=SCVT_None);
}

void MainWindow::on_actionResult_linear_transition_triggered()
{
    pickTransitionType(SCVT_LinearBuff);
}

void MainWindow::on_actionResult_square_transition_triggered()
{
    pickTransitionType(SCVT_SquareBuff);
}

void MainWindow::on_actionVariables_linear_transition_triggered()
{
    pickTransitionType(SCVT_LinearVar);
}

void MainWindow::on_actionVariables_square_transition_triggered()
{
    pickTransitionType(SCVT_SquareVar);
}

void MainWindow::on_actionNo_transition_triggered()
{
    pickTransitionType(SCVT_None);
}

void MainWindow::pickTransitionTime(double timelen)
{
    if (timelen<=0.1) timelen = 0.1;
    ui->action0_5s->setChecked(timelen==0.5);
    ui->action1s->setChecked(timelen==1);
    ui->action2s->setChecked(timelen==2);
    ui->action5s->setChecked(timelen==5);
    ui->action10s->setChecked(timelen==10);
    ui->action30s->setChecked(timelen==30);
    sc->setTransitionTime(timelen);
}

void MainWindow::on_action0_5s_triggered()
{
    pickTransitionTime(0.5);
}

void MainWindow::on_action1s_triggered()
{
    pickTransitionTime(1);
}

void MainWindow::on_action2s_triggered()
{
    pickTransitionTime(2);
}

void MainWindow::on_action5s_triggered()
{
    pickTransitionTime(5);
}

void MainWindow::on_action10s_triggered()
{
    pickTransitionTime(10);
}

void MainWindow::on_action30s_triggered()
{
    pickTransitionTime(30);
}
