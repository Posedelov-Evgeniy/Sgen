#ifndef DIALOGEXPORT_H
#define DIALOGEXPORT_H

#include "../sndcontroller.h"
#include <QDialog>
#include <QAbstractButton>
#include <QTimeEdit>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExport(QWidget *parent = 0);
    ~DialogExport();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushButton_filename_clicked();

    void export_status_changed(int percent);

    void export_finished();

    void accept();
private:
    Ui::DialogExport *ui;

    void exportProcess(int seconds, QString filename);
};

#endif // DIALOGEXPORT_H
