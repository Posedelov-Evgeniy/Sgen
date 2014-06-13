#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    auto_restart = false;
    ui->setupUi(this);
    sc = new SndController(this);
    QObject::connect(sc, SIGNAL(stopped()), this, SLOT(sound_stopped()));
    QObject::connect(sc, SIGNAL(started()), this, SLOT(sound_started()));
    QObject::connect(sc, SIGNAL(starting()), this, SLOT(sound_starting()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_functionsButton_clicked()
{
    ui->functionsButton->setText(ui->functionsButton->isChecked() ? "<" : ">");
    QRect gg = this->geometry();
    gg.setWidth(ui->functionsButton->isChecked() ? 822 : 515);
    this->setGeometry(gg);
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
