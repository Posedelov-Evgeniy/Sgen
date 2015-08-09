#include "variablepickerslist.h"
#include "ui_variablepickerslist.h"

VariablePickersList::VariablePickersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VariablePickersList)
{
    ui->setupUi(this);
}

VariablePickersList::~VariablePickersList()
{
    delete ui;
}

VariablePicker* VariablePickersList::addVariablePicker(QString var_name)
{
    VariablePicker *picker = NULL;
    int length = variables.length();
    if (length<maxVariables)
    {
        QString t;
        picker = new VariablePicker();
        t.sprintf("%08p", picker);
        picker->setObjectName("varpicker"+t);

        ui->variable_container->layout()->addWidget(picker);
        ui->variable_container->adjustSize();

        picker->setVarname(var_name);
        QObject::connect(picker, SIGNAL(add_new(VariablePicker*)), this, SLOT(add_var(VariablePicker*)));
        QObject::connect(picker, SIGNAL(remove_item(VariablePicker*)), this, SLOT(remove_var(VariablePicker*)));
        QObject::connect(picker, SIGNAL(changed()), this, SLOT(options_changing()));
        QObject::connect(picker, SIGNAL(add_new(VariablePicker*)), this, SLOT(options_changing()));
        QObject::connect(picker, SIGNAL(remove_item(VariablePicker*)), this, SLOT(options_changing()));
        QObject::connect(picker, SIGNAL(value_changed(double)), this, SLOT(send_changed_value()));
        variables.append(picker);
        adjustParams();
    }
    return picker;
}

void VariablePickersList::savePickersSettings(QSettings *settings)
{
    VariablePicker *picker;
    settings->setValue("variables/variables_count", variables.length());
    int i = 1;
    foreach(picker, variables) {
        settings->setValue("variables/var"+QString::number(i)+"_name", picker->getVarname());
        settings->setValue("variables/var"+QString::number(i)+"_expression", picker->getVarExpression());
        settings->setValue("variables/var"+QString::number(i)+"_type", (int) picker->getCurrentType());
        settings->setValue("variables/var"+QString::number(i)+"_min", picker->getMinValue());
        settings->setValue("variables/var"+QString::number(i)+"_max", picker->getMaxValue());
        settings->setValue("variables/var"+QString::number(i), picker->getValue());
        i++;
    }
}

void VariablePickersList::loadPickersSettings(QSettings *settings)
{
    VariablePicker *picker;
    foreach(picker, variables) removeVariablePicker(picker);

    int length = settings->value("variables/variables_count", 1).toInt();
    if (length<1) length = 1;
    if (length>maxVariables) length = maxVariables;
    for (int i=1;i<=length;i++) {
        picker = addVariablePicker(settings->value("variables/var"+QString::number(i)+"_name", "").toString());
        if (picker) {
            picker->setVarExpression(settings->value("variables/var"+QString::number(i)+"_expression", "").toString());
            picker->setCurrentType((VariablePicker::VariableDataTypes) settings->value("variables/var"+QString::number(i)+"_type", 0).toInt());
            picker->setMinValue(settings->value("variables/var"+QString::number(i)+"_min", -999999999).toDouble());
            picker->setMaxValue(settings->value("variables/var"+QString::number(i)+"_max", 999999999).toDouble());
            picker->setValue(settings->value("variables/var"+QString::number(i), 0).toDouble(), true);
        }
    }
}

void VariablePickersList::setSNDOptions(SndController *sc)
{
    adjustParams();

    sc->getVariables()->clear();
    sc->getExpressions()->clear();

    for(int i=0; i<variables.length(); i++) {
        if (variables.at(i)->getVarExpression().isEmpty()) {
            sc->getVariables()->insert(variables.at(i)->getVarname(), variables.at(i)->getValue());
        } else {
            sc->getExpressions()->insert(variables.at(i)->getVarname(), variables.at(i)->getVarExpression());
        }
    }
}

void VariablePickersList::removeVariablePicker(VariablePicker *p)
{
    int pos = variables.indexOf(p);
    if (pos>=0) {
        ui->variable_container->layout()->removeWidget(p);
        variables.at(pos)->deleteLater();
        delete variables.at(pos);
        variables.removeAt(pos);
        if (variables.length()==0) {
            addVariablePicker("");
        } else {
            adjustParams();
        }
    }
}

void VariablePickersList::adjustParams()
{
    VariablePicker *picker;
    QList<QString> var_names;
    QString current_var_name;

    int i = 0;
    int j = 1;
    int last = variables.length()-1;
    foreach(picker, variables) {
        picker->setAddButtonEnabled(i==last);
        picker->setRemoveButtonEnabled(true);

        current_var_name = picker->getVarname();
        current_var_name = current_var_name.replace(QRegExp("\\W"), "");
        if (current_var_name.isEmpty() || SndController::Instance()->getInnerVariables()->contains(current_var_name, Qt::CaseInsensitive) || var_names.indexOf(current_var_name)>=0)
        {
            while (var_names.indexOf(current_var_name = "var"+QString::number(j))>=0) j++;
        }
        picker->setVarname(current_var_name);
        var_names.append(current_var_name);
        i++;
    }
}

void VariablePickersList::add_var(VariablePicker *p)
{
    addVariablePicker("");
}

void VariablePickersList::remove_var(VariablePicker *p)
{
    removeVariablePicker(p);
}

void VariablePickersList::options_changing()
{
    emit variables_options_changed();
}

void VariablePickersList::send_changed_value()
{
    VariablePicker *picker = dynamic_cast<VariablePicker*>(this->sender());
    if (picker && SndController::Instance()->getVariables()->contains(picker->getVarname())) {
        SndController::Instance()->setVariable(picker->getVarname(), picker->getValue());
    }
}
