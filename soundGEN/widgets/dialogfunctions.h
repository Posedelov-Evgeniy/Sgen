#ifndef DIALOGFUNCTIONS_H
#define DIALOGFUNCTIONS_H

#include <QDialog>
#include <QMainWindow>
#include <QAbstractButton>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include "./functiongraphicdrawer.h"

namespace Ui {
class DialogFunctions;
}

struct DialogFunctionRecord {
    QString name;
    QString description;
    QString category;
};

class DialogFunctions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFunctions(QWidget *parent = 0);
    ~DialogFunctions();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::DialogFunctions *ui;
    QVector<DialogFunctionRecord> *records;
    void initRecords();
};

#endif // DIALOGFUNCTIONS_H
