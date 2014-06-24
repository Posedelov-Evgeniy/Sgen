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
    base_width = QWidget::width();
    sc = SndController::Instance();

    ui->lineEdit_sound1->setText(settings.value("main/sound", "").toString());

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

    settings.setValue("main/sound", ui->lineEdit_sound1->text());
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
    ui->functionsButton->setText(ui->functionsButton->isChecked() ? "<" : ">");
    QWidget::resize(ui->functionsButton->isChecked() ? base_width + ui->plainTextEdit_user_functions->width()+15 : base_width, QWidget::height());
    panel_opened = !panel_opened;
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
    sc->SetSoundFile(ui->lineEdit_sound1->text());
    sc->SetLAmp(ui->doubleSpinBox_amp_left->value());
    sc->SetRAmp(ui->doubleSpinBox_amp_right->value());
    sc->SetLFreq(ui->doubleSpinBox_freq_left->value());
    sc->SetRFreq(ui->doubleSpinBox_freq_right->value());
    sc->SetFunctionsStr(ui->plainTextEdit_user_functions->document()->toPlainText());
}

void MainWindow::sound_stopped()
{
    ui->pushButton_play->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_restart->setEnabled(false);
    if (auto_restart) {
        auto_restart = false;
        sc->run();
    }
}

void MainWindow::sound_started()
{
    ui->pushButton_play->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_restart->setEnabled(true);
}

void MainWindow::on_MainWindow_destroyed()
{
    sc->stop();
}

void MainWindow::on_pushButton_sound1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Sound files (*.mp3 *.wav *.ogg)"));
    ui->lineEdit_sound1->setText(fileName);
}

void MainWindow::on_pushButton_play_clicked()
{
    sc->run();
}

void MainWindow::on_pushButton_stop_clicked()
{
    sc->stop();
}

void MainWindow::on_pushButton_restart_clicked()
{
    auto_restart = true;
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
