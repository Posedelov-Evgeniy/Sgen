#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/config.cfg", QSettings::IniFormat);
    auto_restart = false;
    panel_opened = false;
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Запуск");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Стоп");
    ui->buttonBox->button(QDialogButtonBox::Retry)->setText("Перезапуск");
    sound_stopped();

    ui->plainTextEdit_user_functions->setVisible(panel_opened);
    base_width = QWidget::width();
    base_height = QWidget::height();
    //base_controls_top = ui->groupBox_controls_buttons->pos().y();
    //base_sounds_top = base_controls_top - 5;
    base_functions_button_height = ui->functionsButton->height();
    sc = SndController::Instance();

    int length = settings.value("main/sounds_count", 1).toInt();
    if (length<1) length = 1;
    if (length>maxSounds) length = maxSounds;
    for (int i=1;i<=length;i++) {
        addSoundPicker(settings.value("main/sound"+QString::number(i), "").toString());
    }

    ui->lineEdit_function_left->setText(settings.value("main/function_l", "sin(k*t)").toString());
    ui->lineEdit_function_right->setText(settings.value("main/function_r", "cos(k*t)").toString());
    ui->doubleSpinBox_amp_left->setValue(settings.value("main/amp_l", 1).toDouble());
    ui->doubleSpinBox_amp_right->setValue(settings.value("main/amp_r", 1).toDouble());
    ui->doubleSpinBox_freq_left->setValue(settings.value("main/freq_l", 500).toDouble());
    ui->doubleSpinBox_freq_right->setValue(settings.value("main/freq_r", 500).toDouble());

    if (settings.value("window/f_panel_opened", 0).toInt()==1) {
        ui->functionsButton->click();
    }

    QWidget::move(settings.value("window/left", 100).toInt(), settings.value("window/top", 100).toInt());

    QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp");
    if(!file.exists()){
        qDebug() << "Functions.cpp not exists";
    }
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString funct_str = stream.readAll().toUtf8();
        ui->plainTextEdit_user_functions->document()->setPlainText(funct_str);
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

    SoundPicker *picker;
    settings.setValue("main/sounds_count", sounds.length());
    int i = 1;
    foreach(picker, sounds) {
        settings.setValue("main/sound"+QString::number(i), picker->getFilename());
        i++;
    }

    settings.setValue("main/function_l", ui->lineEdit_function_left->text());
    settings.setValue("main/function_r", ui->lineEdit_function_right->text());
    settings.setValue("main/amp_l", ui->doubleSpinBox_amp_left->value());
    settings.setValue("main/amp_r", ui->doubleSpinBox_amp_right->value());
    settings.setValue("main/freq_l", ui->doubleSpinBox_freq_left->value());
    settings.setValue("main/freq_r", ui->doubleSpinBox_freq_right->value());

    settings.setValue("window/f_panel_opened", panel_opened ? 1 : 0);

    QRect gg = this->geometry();
    settings.setValue("window/left", gg.left());
    settings.setValue("window/top", gg.top());

    QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp");
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << ui->plainTextEdit_user_functions->document()->toPlainText();
        file.close();
        if (stream.status() != QTextStream::Ok)
        {
            qDebug() << "Error writing functions.cpp";
        }
    }

    delete ui;
}

void MainWindow::on_functionsButton_clicked()
{
    panel_opened = !panel_opened;
    ui->plainTextEdit_user_functions->setVisible(panel_opened);
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
    sc->SetLFunctionStr(ui->lineEdit_function_left->text());
    sc->SetRFunctionStr(ui->lineEdit_function_right->text());
    sc->SetSoundFile(sounds.first()->getFilename());
    sc->SetLAmp(ui->doubleSpinBox_amp_left->value());
    sc->SetRAmp(ui->doubleSpinBox_amp_right->value());
    sc->SetLFreq(ui->doubleSpinBox_freq_left->value());
    sc->SetRFreq(ui->doubleSpinBox_freq_right->value());
    sc->SetFunctionsStr(ui->plainTextEdit_user_functions->document()->toPlainText());
}

void MainWindow::sound_stopped()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(false);
    if (auto_restart) {
        auto_restart = false;
        sc->run();
    }
}

void MainWindow::sound_started()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Retry)->setEnabled(true);
}

void MainWindow::on_MainWindow_destroyed()
{
    sc->stop();
}

void MainWindow::on_doubleSpinBox_amp_left_valueChanged(double arg1)
{
    sc->SetLAmp(arg1);
}

void MainWindow::on_doubleSpinBox_amp_right_valueChanged(double arg1)
{
    sc->SetRAmp(arg1);
}

void MainWindow::on_doubleSpinBox_freq_left_valueChanged(double arg1)
{
    sc->SetLFreq(arg1);
}

void MainWindow::on_doubleSpinBox_freq_right_valueChanged(double arg1)
{
    sc->SetRFreq(arg1);
}

void MainWindow::addSoundPicker(QString fname)
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

        picker->setFilename(fname);
        QObject::connect(picker, SIGNAL(add_new(SoundPicker*)), this, SLOT(add_sound(SoundPicker*)));
        QObject::connect(picker, SIGNAL(remove_item(SoundPicker*)), this, SLOT(remove_sound(SoundPicker*)));
        sounds.append(picker);
        adjustSoundSizes();
    }
}

void MainWindow::removeSoundPicker(SoundPicker *p)
{
    if (sounds.length()>1) {
        if (int pos = sounds.indexOf(p)) {
            sounds.at(pos)->deleteLater();
            delete sounds.at(pos);
            sounds.removeAt(pos);
            adjustSoundSizes();
        }
    }
}

void MainWindow::adjustSoundSizes()
{
    /*int def_height = sounds.isEmpty() ? 0 : sounds.length() * sounds.first()->height();
    setFixedHeight(base_height + def_height);

    QPoint pp;
    QRect rr;

    pp = ui->groupBox_controls_buttons->pos();
    ui->groupBox_controls_buttons->move(pp.x(), base_controls_top + def_height);

    rr = ui->functionsButton->geometry();
    rr.setHeight(base_functions_button_height + def_height);
    ui->functionsButton->setGeometry(rr);*/

    SoundPicker *picker;
    int i = 0;
    int last = sounds.length()-1;
    foreach(picker, sounds) {
        picker->move(20, base_sounds_top + i * picker->height());
        picker->setAddButtonEnabled(i==last);
        picker->setRemoveButtonEnabled(last>0);
        i++;
    }
}

void MainWindow::add_sound(SoundPicker *p)
{
    addSoundPicker("");
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
