#include "dialogexport.h"
#include "ui_dialogexport.h"

DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QObject::connect(SndController::Instance(),SIGNAL(export_finished()),this,SLOT(export_finished()));
    QObject::connect(SndController::Instance(),SIGNAL(export_status(int)),this,SLOT(export_status_changed(int)));
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button == ui->buttonBox->button(QDialogButtonBox::Save))
    {
        QTime tedit = ui->timeEdit->time();
        int seconds = tedit.second() + tedit.minute()*60 + tedit.hour()*3600;
        if (seconds>0) {
            if (ui->filenameEdit->text().size()>0) {
                exportProcess(seconds, ui->filenameEdit->text());
            } else {
                QMessageBox::critical(this, tr("Export"), tr("You need to set export file!"), QMessageBox::Ok, QMessageBox::Ok);
            }
        } else {
            QMessageBox::critical(this, tr("Export"), tr("You need to set duration to at least 1 second!"), QMessageBox::Ok, QMessageBox::Ok);
        }
    } else {
        close();
    }
}

void DialogExport::on_pushButton_filename_clicked()
{
    QString bfile = EnvironmentInfo::getHomePath();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), bfile, tr("Sound file (*.wav)"));
    ui->filenameEdit->setText(fileName);
}

void DialogExport::exportProcess(int seconds, QString filename)
{
    ui->buttonBox->setEnabled(false);
    ui->filenameEdit->setEnabled(false);
    ui->timeEdit->setEnabled(false);
    ui->label_filename->setEnabled(false);
    ui->label_timeEdit->setEnabled(false);
    SndController::Instance()->run_export(seconds, filename);
}

void DialogExport::export_status_changed(int percent)
{
    ui->progressBar->setValue(percent);
}

void DialogExport::export_finished()
{
    ui->buttonBox->setEnabled(true);
    ui->filenameEdit->setEnabled(true);
    ui->timeEdit->setEnabled(true);
    ui->label_filename->setEnabled(true);
    ui->label_timeEdit->setEnabled(true);
    QMessageBox::information(this, tr("Export"), tr("Export successfully finished!"), QMessageBox::Ok, QMessageBox::Ok);
    close();
}

void DialogExport::accept()
{
    return;
}
