#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/config.cfg", QSettings::IniFormat);
    auto_restart = false;
    ui->setupUi(this);

    dialog_functions = new DialogFunctions(this);

    left_drawer = new functionGraphicDrawer();
    right_drawer = new functionGraphicDrawer();

    ui->verticalLayout_drawers->addWidget(left_drawer);
    ui->verticalLayout_drawers->addWidget(right_drawer);

    functions_text = new UTextEdit();
    ui->functions_tab->layout()->addWidget(functions_text);

    left_function = new UTextEdit();
    ui->left_function_layout->insertWidget(1, left_function);

    right_function = new UTextEdit();
    ui->right_function_layout->insertWidget(1, right_function);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(" Запуск ");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(" Стоп ");
    ui->buttonBox->button(QDialogButtonBox::Retry)->setText(" Перезапуск ");
    sound_stopped();
    sc = SndController::Instance();

    int length = settings.value("sounds/sounds_count", 1).toInt();
    if (length<1) length = 1;
    if (length>maxSounds) length = maxSounds;
    for (int i=1;i<=length;i++) {
        addSoundPicker(settings.value("sounds/sound"+QString::number(i), "").toString(), settings.value("sounds/sound"+QString::number(i)+"_function", "").toString());
    }

    left_function->document()->setPlainText(settings.value("main/function_l", "sin(k*t)").toString());
    right_function->document()->setPlainText(settings.value("main/function_r", "cos(k*t)").toString());
    ui->doubleSpinBox_amp_left->setValue(settings.value("main/amp_l", 1).toDouble());
    ui->doubleSpinBox_amp_right->setValue(settings.value("main/amp_r", 1).toDouble());
    ui->doubleSpinBox_freq_left->setValue(settings.value("main/freq_l", 500).toDouble());
    ui->doubleSpinBox_freq_right->setValue(settings.value("main/freq_r", 500).toDouble());

    left_drawer->setKampIntValue(settings.value("graphic/kamp_l", 0).toDouble());
    right_drawer->setKampIntValue(settings.value("graphic/kamp_r", 0).toDouble());
    left_drawer->setDtIntValue(settings.value("graphic/dt_l", 300).toDouble());
    right_drawer->setDtIntValue(settings.value("graphic/dt_r", 300).toDouble());

    QWidget::move(settings.value("window/left", 100).toInt(), settings.value("window/top", 100).toInt());
    QWidget::resize(settings.value("window/width", 400).toInt(), settings.value("window/height", 300).toInt());

    QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp.cfg");
    if(!file.exists()){
        qDebug() << "Functions.cpp.cfg not exists";
    }
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString funct_str = stream.readAll().toUtf8();
        functions_text->document()->setPlainText(funct_str);
    }

    QObject::connect(sc, SIGNAL(stopped()), this, SLOT(sound_stopped()));
    QObject::connect(sc, SIGNAL(started()), this, SLOT(sound_started()));
    QObject::connect(sc, SIGNAL(starting()), this, SLOT(sound_starting()));
    QObject::connect(sc, SIGNAL(cycle_start()), this, SLOT(cycle_starting()));
    QObject::connect(sc, SIGNAL(write_message(QString)), this, SLOT(get_message(QString)));
}

MainWindow::~MainWindow()
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/config.cfg", QSettings::IniFormat);

    settings.setValue("main/function_l", left_function->document()->toPlainText());
    settings.setValue("main/function_r", right_function->document()->toPlainText());
    settings.setValue("main/amp_l", ui->doubleSpinBox_amp_left->value());
    settings.setValue("main/amp_r", ui->doubleSpinBox_amp_right->value());
    settings.setValue("main/freq_l", ui->doubleSpinBox_freq_left->value());
    settings.setValue("main/freq_r", ui->doubleSpinBox_freq_right->value());

    settings.setValue("graphic/kamp_l", left_drawer->getKampIntValue());
    settings.setValue("graphic/kamp_r", right_drawer->getKampIntValue());
    settings.setValue("graphic/dt_l", left_drawer->getDtIntValue());
    settings.setValue("graphic/dt_r", right_drawer->getDtIntValue());

    SoundPicker *picker;
    settings.setValue("sounds/sounds_count", sounds.length());
    int i = 1;
    foreach(picker, sounds) {
        settings.setValue("sounds/sound"+QString::number(i), picker->getFilename());
        settings.setValue("sounds/sound"+QString::number(i)+"_function", picker->getFunctionname());
        i++;
    }

    QRect gg = this->geometry();
    settings.setValue("window/left", gg.left());
    settings.setValue("window/top", gg.top());
    settings.setValue("window/width", gg.width());
    settings.setValue("window/height", gg.height());

    QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp.cfg");
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << functions_text->document()->toPlainText();
        file.close();
        if (stream.status() != QTextStream::Ok)
        {
            qDebug() << "Error writing functions.cpp.cfg";
        }
    }

    left_drawer->deleteLater();
    right_drawer->deleteLater();
    functions_text->deleteLater();
    delete left_drawer;
    delete right_drawer;
    delete functions_text;
    delete left_function;
    delete right_function;
    delete dialog_functions;
    delete ui;
}

void MainWindow::cycle_starting()
{
    ui->lcdNumber_freq_left->display(sc->getInstLFreq());
    ui->lcdNumber_freq_right->display(sc->getInstRFreq());
    ui->lcdNumber_amp_left->display(sc->getInstLAmp());
    ui->lcdNumber_amp_right->display(sc->getInstRAmp());
}

void MainWindow::get_message(QString message)
{
    ui->statusBar->showMessage(message, 5000);
}

void MainWindow::sound_starting()
{
    sc->SetLFunctionStr(left_function->document()->toPlainText());
    sc->SetRFunctionStr(right_function->document()->toPlainText());
    sc->SetLAmp(ui->doubleSpinBox_amp_left->value());
    sc->SetRAmp(ui->doubleSpinBox_amp_right->value());
    sc->SetLFreq(ui->doubleSpinBox_freq_left->value());
    sc->SetRFreq(ui->doubleSpinBox_freq_right->value());
    sc->SetFunctionsStr(functions_text->document()->toPlainText());

    left_drawer->setAmp(ui->doubleSpinBox_amp_left->value());
    right_drawer->setAmp(ui->doubleSpinBox_amp_right->value());
    left_drawer->setFreq(ui->doubleSpinBox_freq_left->value());
    right_drawer->setFreq(ui->doubleSpinBox_freq_right->value());

    SoundPicker *picker;
    foreach(picker, sounds) {
        sc->AddSound(picker->getFilename(), picker->getFunctionname());
    }
}

void MainWindow::sound_stopped()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(false);
    if (auto_restart) {
        auto_restart = false;
        sc->run();
    } else {
        left_drawer->stop();
        right_drawer->stop();
    }
}

void MainWindow::sound_started()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(true);

    left_drawer->setGraphicFunction(sc->getLeftFunction());
    right_drawer->setGraphicFunction(sc->getRightFunction());
    left_drawer->run();
    right_drawer->run();
}

void MainWindow::on_MainWindow_destroyed()
{
    sc->stop();
}

void MainWindow::on_doubleSpinBox_amp_left_valueChanged(double arg1)
{
    sc->SetLAmp(arg1);
    left_drawer->setAmp(arg1);
}

void MainWindow::on_doubleSpinBox_amp_right_valueChanged(double arg1)
{
    sc->SetRAmp(arg1);
    right_drawer->setAmp(arg1);
}

void MainWindow::on_doubleSpinBox_freq_left_valueChanged(double arg1)
{
    sc->SetLFreq(arg1);
    left_drawer->setFreq(arg1);
}

void MainWindow::on_doubleSpinBox_freq_right_valueChanged(double arg1)
{
    sc->SetRFreq(arg1);
    right_drawer->setFreq(arg1);
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
        sounds.append(picker);
        adjustSoundParams();
    }
}

void MainWindow::removeSoundPicker(SoundPicker *p)
{
    if (sounds.length()>1) {
        if (int pos = sounds.indexOf(p)) {
            sounds.at(pos)->deleteLater();
            delete sounds.at(pos);
            sounds.removeAt(pos);
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
        picker->setRemoveButtonEnabled(last>0);

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

void MainWindow::on_left_dialog_functions_btn_clicked()
{
    dialog_functions->exec();
}

void MainWindow::on_right_dialog_functions_btn_clicked()
{
    dialog_functions->exec();
}
