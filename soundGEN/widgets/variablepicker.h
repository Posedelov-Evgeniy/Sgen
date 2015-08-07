#ifndef VARIABLEPICKER_H
#define VARIABLEPICKER_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "../sndcontroller.h"

namespace Ui {
class VariablePicker;
}

class VariablePicker : public QWidget
{
    Q_OBJECT

public:
    enum VariableDataTypes {
        VDT_INT,
        VDT_DOUBLE,
        VDT_TIME,
        VDT_BOOL
    };

    explicit VariablePicker(QWidget *parent = 0, bool add_button = true, bool remove_button = true);
    ~VariablePicker();
    void setAddButtonEnabled(bool enabled);
    void setRemoveButtonEnabled(bool enabled);

    void setVarname(QString varname);
    QString getVarname();

    void setVarExpression(QString exp_str);
    QString getVarExpression();

    void setValue(double value);
    void setValue(double value, bool hide_types);
    double getValue();

    VariablePicker::VariableDataTypes getCurrentType() const;
    void setCurrentType(VariablePicker::VariableDataTypes new_type);

    double getMinValue();
    void setMinValue(double minval);
    double getMaxValue();
    void setMaxValue(double maxval);

signals:
    void remove_item(VariablePicker *p);
    void add_new(VariablePicker *p);
    void changed();
    void value_changed(double new_value);

private slots:
    void on_pushButton_sound_remove_clicked();
    void on_pushButton_sound_add_clicked();
    void params_changed();
    void updValue();

    void on_pushButton_type_clicked(bool checked);
    void on_pushButton_expression_clicked(bool checked);
    void on_comboBox_type_currentIndexChanged(int index);
    void on_spinBox_min_valueChanged(int arg1);
    void on_doubleSpinBox_min_valueChanged(double arg1);
    void on_spinBox_max_valueChanged(int arg1);
    void on_doubleSpinBox_max_valueChanged(double arg1);

private:
    Ui::VariablePicker *ui;
    int self_index;
    double var_value;
    VariableDataTypes current_type;
};

#endif // VARIABLEPICKER_H
