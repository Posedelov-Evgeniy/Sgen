#ifndef VARIABLEPICKERSLIST_H
#define VARIABLEPICKERSLIST_H

#include <QWidget>
#include "variablepicker.h"

namespace Ui {
class VariablePickersList;
}

class VariablePickersList : public QWidget
{
    Q_OBJECT

public:
    explicit VariablePickersList(QWidget *parent = 0);
    ~VariablePickersList();

    void savePickersSettings(QSettings *settings);
    void loadPickersSettings(QSettings *settings);
    void setSNDOptions(SndController* sc);

signals:
    void variables_options_changed();

private slots:
    void add_var(VariablePicker* p);
    void remove_var(VariablePicker* p);
    void options_changing();
    void send_changed_value();

private:
    Ui::VariablePickersList *ui;
    static const int maxVariables = 10;
    QList<VariablePicker*> variables;

    VariablePicker* addVariablePicker(QString var_name);
    void removeVariablePicker(VariablePicker* p);
    void adjustParams();
};

#endif // VARIABLEPICKERSLIST_H
