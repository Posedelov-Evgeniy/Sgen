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
    sc = SndController::Instance();

    ui->lineEdit->setText(settings.value("main/function_l", "sin(k*t)").toString());
    ui->lineEdit_2->setText(settings.value("main/function_r", "cos(k*t)").toString());
    ui->doubleSpinBox->setValue(settings.value("main/amp_l", 1).toDouble());
    ui->doubleSpinBox_3->setValue(settings.value("main/amp_r", 1).toDouble());
    ui->doubleSpinBox_2->setValue(settings.value("main/freq_l", 500).toDouble());
    ui->doubleSpinBox_4->setValue(settings.value("main/freq_r", 500).toDouble());

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
        ui->plainTextEdit->document()->setPlainText(funct_str);
    }

    QObject::connect(sc, SIGNAL(stopped()), this, SLOT(sound_stopped()));
    QObject::connect(sc, SIGNAL(started()), this, SLOT(sound_started()));
    QObject::connect(sc, SIGNAL(starting()), this, SLOT(sound_starting()));
    QObject::connect(sc, SIGNAL(cycle_start()), this, SLOT(cycle_starting()));
}

MainWindow::~MainWindow()
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/config.cfg", QSettings::IniFormat);
    settings.setValue("main/function_l", ui->lineEdit->text());
    settings.setValue("main/function_r", ui->lineEdit_2->text());
    settings.setValue("main/amp_l", ui->doubleSpinBox->value());
    settings.setValue("main/amp_r", ui->doubleSpinBox_3->value());
    settings.setValue("main/freq_l", ui->doubleSpinBox_2->value());
    settings.setValue("main/freq_r", ui->doubleSpinBox_4->value());

    settings.setValue("window/f_panel_opened", panel_opened ? 1 : 0);

    QRect gg = this->geometry();
    settings.setValue("window/left", gg.left());
    settings.setValue("window/top", gg.top());

    QFile file(QCoreApplication::applicationDirPath()+"/functions.cpp");
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << ui->plainTextEdit->document()->toPlainText();
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
    QRect gg = this->geometry();
    gg.setWidth(ui->functionsButton->isChecked() ? 822 : 515);
    this->setGeometry(gg);
    panel_opened = !panel_opened;
}

void MainWindow::on_pushButton_clicked()
{
    sc->run();
}

void MainWindow::on_pushButton_2_clicked()
{
    sc->stop();
}

void MainWindow::on_pushButton_3_clicked()
{
    auto_restart = true;
    sc->stop();
}

void MainWindow::cycle_starting()
{
    ui->lcdNumber_2->display(sc->getInstLFreq());
    ui->lcdNumber_4->display(sc->getInstRFreq());
    ui->lcdNumber->display(sc->getInstLAmp());
    ui->lcdNumber_3->display(sc->getInstRAmp());
}

void MainWindow::sound_starting()
{
    sc->SetLFunctionStr(ui->lineEdit->text());
    sc->SetRFunctionStr(ui->lineEdit_2->text());
    sc->SetLAmp(ui->doubleSpinBox->value());
    sc->SetRAmp(ui->doubleSpinBox_3->value());
    sc->SetLFreq(ui->doubleSpinBox_2->value());
    sc->SetRFreq(ui->doubleSpinBox_4->value());
    sc->SetFunctionsStr(ui->plainTextEdit->document()->toPlainText());
}

void MainWindow::sound_stopped()
{
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    if (auto_restart) {
        auto_restart = false;
        sc->run();
    }
}

void MainWindow::sound_started()
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    sc->SetLAmp(arg1);
}

void MainWindow::on_doubleSpinBox_3_valueChanged(double arg1)
{
    sc->SetRAmp(arg1);
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    sc->SetLFreq(arg1);
}

void MainWindow::on_doubleSpinBox_4_valueChanged(double arg1)
{
    sc->SetRFreq(arg1);
}

void MainWindow::on_MainWindow_destroyed()
{
    sc->stop();
}
