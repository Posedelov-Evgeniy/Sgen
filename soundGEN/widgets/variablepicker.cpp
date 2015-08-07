#include "variablepicker.h"
#include "ui_variablepicker.h"

VariablePicker::VariablePicker(QWidget *parent, bool add_button, bool remove_button) :
    QWidget(parent),
    ui(new Ui::VariablePicker)
{
    current_type = VDT_INT;
    var_value = 0;
    ui->setupUi(this);
    ui->pushButton_sound_add->setEnabled(add_button);
    ui->pushButton_sound_remove->setEnabled(remove_button);
    connect(ui->lineEdit_value, SIGNAL(textChanged(QString)), this, SLOT(params_changed()));
    connect(ui->lineEdit_function, SIGNAL(textChanged(QString)), this, SLOT(params_changed()));

    connect(ui->spinBox_value_int, SIGNAL(valueChanged(QString)), this, SLOT(params_changed()));
    connect(ui->spinBox_value_int, SIGNAL(valueChanged(QString)), this, SLOT(updValue()));
    connect(ui->doubleSpinBox_value_double, SIGNAL(valueChanged(QString)), this, SLOT(params_changed()));
    connect(ui->doubleSpinBox_value_double, SIGNAL(valueChanged(QString)), this, SLOT(updValue()));
    connect(ui->timeEdit_value_time, SIGNAL(timeChanged(QTime)), this, SLOT(params_changed()));
    connect(ui->timeEdit_value_time, SIGNAL(timeChanged(QTime)), this, SLOT(updValue()));
    connect(ui->pushButton_value_bool, SIGNAL(clicked()), this, SLOT(params_changed()));
    connect(ui->pushButton_value_bool, SIGNAL(clicked()), this, SLOT(updValue()));

    connect(ui->doubleSpinBox_min, SIGNAL(valueChanged(QString)), this, SLOT(params_changed()));
    connect(ui->doubleSpinBox_max, SIGNAL(valueChanged(QString)), this, SLOT(params_changed()));

    on_comboBox_type_currentIndexChanged(0);
}

VariablePicker::~VariablePicker()
{
    delete ui;
}

void VariablePicker::setAddButtonEnabled(bool enabled)
{
    ui->pushButton_sound_add->setEnabled(enabled);
}

void VariablePicker::setRemoveButtonEnabled(bool enabled)
{
    ui->pushButton_sound_remove->setEnabled(enabled);
}

void VariablePicker::setVarname(QString varname)
{
    ui->lineEdit_function->setText(varname);
}

QString VariablePicker::getVarname()
{
    return ui->lineEdit_function->text();
}

void VariablePicker::setVarExpression(QString exp_str)
{
    ui->lineEdit_value->setText(exp_str);
    if (!exp_str.isEmpty() && !ui->pushButton_expression->isChecked()) {
        ui->pushButton_expression->setChecked(true);
        on_pushButton_expression_clicked(true);
    }
}

QString VariablePicker::getVarExpression()
{
    return ui->pushButton_expression->isChecked() ? ui->lineEdit_value->text() : "";
}

void VariablePicker::setValue(double value)
{
    switch(current_type) {
        case VDT_DOUBLE:
            ui->doubleSpinBox_value_double->setValue(value);
            break;
        case VDT_TIME:
            ui->timeEdit_value_time->setTime(QTime::fromMSecsSinceStartOfDay(round(value)));
            break;
        case VDT_BOOL:
            ui->pushButton_value_bool->setChecked(round(value)>0);
            break;
        default:
            ui->spinBox_value_int->setValue(round(value));
    }
    var_value = value;
}

void VariablePicker::setValue(double value, bool hide_types)
{
    setValue(value);
    if (hide_types) {
        ui->pushButton_type->setChecked(false);
        on_pushButton_type_clicked(false);
    }
}

double VariablePicker::getValue()
{
    return var_value;
}

void VariablePicker::on_pushButton_sound_remove_clicked()
{
    emit remove_item(this);
}

void VariablePicker::on_pushButton_sound_add_clicked()
{
    emit add_new(this);
}

void VariablePicker::params_changed()
{
    emit changed();
}

void VariablePicker::on_pushButton_type_clicked(bool checked)
{
    ui->widget_type->setVisible(checked);
    ui->widget_min->setVisible(checked && (current_type==VDT_DOUBLE || current_type==VDT_INT));
    ui->widget_max->setVisible(checked && (current_type==VDT_DOUBLE || current_type==VDT_INT));
}

void VariablePicker::on_pushButton_expression_clicked(bool checked)
{
    ui->lineEdit_value->setVisible(checked);
    ui->doubleSpinBox_value_double->setVisible(!checked && current_type==VDT_DOUBLE);
    ui->spinBox_value_int->setVisible(!checked && current_type==VDT_INT);
    ui->timeEdit_value_time->setVisible(!checked && current_type==VDT_TIME);
    ui->pushButton_value_bool->setVisible(!checked && current_type==VDT_BOOL);
}

void VariablePicker::on_comboBox_type_currentIndexChanged(int index)
{
    switch(index) {
        case 1: current_type = VDT_DOUBLE; break;
        case 2: current_type = VDT_TIME; break;
        case 3: current_type = VDT_BOOL; break;
        default: current_type = VDT_INT;
    }
    setValue(var_value);
    ui->spinBox_min->setVisible(current_type == VDT_INT);
    ui->spinBox_max->setVisible(current_type == VDT_INT);
    ui->doubleSpinBox_min->setVisible(current_type == VDT_DOUBLE);
    ui->doubleSpinBox_max->setVisible(current_type == VDT_DOUBLE);
    on_pushButton_type_clicked(ui->pushButton_type->isChecked());
    on_pushButton_expression_clicked(ui->pushButton_expression->isChecked());
}

VariablePicker::VariableDataTypes VariablePicker::getCurrentType() const
{
    return current_type;
}

void VariablePicker::setCurrentType(VariablePicker::VariableDataTypes new_type)
{
    on_comboBox_type_currentIndexChanged((int) new_type);
}

double VariablePicker::getMinValue()
{
    if (current_type == VDT_DOUBLE) {
        return ui->doubleSpinBox_min->value();
    } else {
        return ui->spinBox_min->value();
    }
}

void VariablePicker::setMinValue(double minval)
{
    ui->doubleSpinBox_min->setValue(minval);
    on_doubleSpinBox_min_valueChanged(minval);
}

double VariablePicker::getMaxValue()
{
    if (current_type == VDT_DOUBLE) {
        return ui->doubleSpinBox_max->value();
    } else {
        return ui->spinBox_max->value();
    }
}

void VariablePicker::setMaxValue(double maxval)
{
    ui->doubleSpinBox_max->setValue(maxval);
    on_doubleSpinBox_max_valueChanged(maxval);
}

void VariablePicker::on_spinBox_min_valueChanged(int arg1)
{
    ui->doubleSpinBox_min->setValue(arg1);
    ui->doubleSpinBox_value_double->setMinimum(arg1);
    ui->spinBox_value_int->setMinimum(arg1);
}

void VariablePicker::on_doubleSpinBox_min_valueChanged(double arg1)
{
    ui->spinBox_min->setValue(round(arg1));
    ui->doubleSpinBox_value_double->setMinimum(arg1);
    ui->spinBox_value_int->setMinimum(round(arg1));
}

void VariablePicker::on_spinBox_max_valueChanged(int arg1)
{
    ui->doubleSpinBox_max->setValue(arg1);
    ui->doubleSpinBox_value_double->setMaximum(arg1);
    ui->spinBox_value_int->setMaximum(arg1);
}

void VariablePicker::on_doubleSpinBox_max_valueChanged(double arg1)
{
    ui->spinBox_max->setValue(round(arg1));
    ui->doubleSpinBox_value_double->setMaximum(arg1);
    ui->spinBox_value_int->setMaximum(round(arg1));
}

void VariablePicker::updValue()
{
    double last_val = var_value;

    switch(current_type) {
        case VDT_DOUBLE:
            var_value = ui->doubleSpinBox_value_double->value();
            break;
        case VDT_TIME:
            var_value = ui->timeEdit_value_time->time().msecsSinceStartOfDay();
            break;
        case VDT_BOOL:
            var_value = ui->pushButton_value_bool->isChecked() ? 1 : 0;
            break;
        default:
            var_value = ui->spinBox_value_int->value();
    }

    if (last_val != var_value) emit value_changed(var_value);
}
