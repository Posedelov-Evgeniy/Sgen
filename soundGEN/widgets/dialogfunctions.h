#ifndef DIALOGFUNCTIONS_H
#define DIALOGFUNCTIONS_H

#include <QDialog>
#include <QMainWindow>
#include <QAbstractButton>
#include <QFileDialog>
#include <QTextStream>
#include <QPushButton>
#include <QFile>
#include "./functiongraphicdrawer.h"
#include "../base_functions.h"

namespace Ui {
class DialogFunctions;
}

struct DialogFunctionRecord {
    QString name;
    QString description;
    QString category;
    base_function_signal function;
};

class DialogFunctions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFunctions(QWidget *parent = 0);
    ~DialogFunctions();
    QString getPickedFunction();

private slots:
    void on_functions_listWidget_currentRowChanged(int currentRow);
    void on_DialogFunctions_finished(int result);
    void on_functions_listWidget_activated(const QModelIndex &index);

    void on_category_comboBox_currentIndexChanged(int index);

private:
    Ui::DialogFunctions *ui;
    QVector<DialogFunctionRecord> *records;
    MGraphicDrawSurface *widget_drawer;
    int last_picked_row;
    void initRecords();
};

#endif // DIALOGFUNCTIONS_H
