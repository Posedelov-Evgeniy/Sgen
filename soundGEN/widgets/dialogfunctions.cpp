#include "dialogfunctions.h"
#include "ui_dialogfunctions.h"

DialogFunctions::DialogFunctions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFunctions)
{
    ui->setupUi(this);

    widget_drawer = new MGraphicDrawSurface();
    widget_drawer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    ui->description_layout->addWidget(widget_drawer);
    widget_drawer->setT(0);
    widget_drawer->setT0(0);
    widget_drawer->setFreq(500);
    widget_drawer->setAmp(1);
    widget_drawer->setKamp(1);
    widget_drawer->setKt(0.05);
    widget_drawer->setDt(0.008);
    widget_drawer->resetGraphicFunctions();
    widget_drawer->setMinimumHeight(115);

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    ui->buttonBox->setLayoutDirection(Qt::LeftToRight);
    #endif
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Add"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initRecords();
}

void DialogFunctions::initRecords()
{
    std::vector<base_function_def> base_f;
    base_function_def curr_def;
    getBaseFunctions(&base_f);

    QString funct_str;
    QFile file(QCoreApplication::applicationDirPath()+"/base_functions.cpp");
    if(!file.exists()){
        qDebug() << tr("base_functions.cpp not exists");
    }
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        funct_str = stream.readAll();
    }
    if (funct_str.isEmpty()) return;

    records = new QVector<DialogFunctionRecord>();

    QRegExp rx("/\\*(.*)\\*/");
    QString str;
    int pos, i;
    pos = 0;

    rx.setMinimal(true);
    while((pos = rx.indexIn(funct_str, pos)) != -1)
    {
        DialogFunctionRecord rec;
        QString base_str = QString(rx.cap(1));
        QStringList base_recs = QStringList(base_str.split('\n'));
        while (base_recs.length() && base_recs.first().trimmed().isEmpty()) base_recs.removeFirst();

        rec.function = 0;

        for(i=0; i<base_recs.length(); i++) {
            str = base_recs.at(i);
            base_recs[i] = str.replace(QRegExp("^(\\s){1,}"), "");
        }

        if (base_recs.length()>=1) {
            str = base_recs.at(0);
            rec.name = str.replace('{',' ').replace('}',' ').trimmed();
            base_recs[0] = "";
            QString function_name = QString(rec.name).replace(QRegExp("\\(.*\\)"),"");

            foreach(curr_def, base_f) {
                if (function_name==QString(curr_def.name.data())) {
                    rec.function = curr_def.function;
                    break;
                }
            }
        }

        if (base_recs.length()>=2) {
            rec.category = base_recs.at(1);
            rec.category = rec.category.replace(QRegExp("^(\\s){0,}Категория:(\\s){0,}"), "");
            rec.category = rec.category.replace(QRegExp(".(\\s){0,}$"), "");
            rec.category = rec.category.left(1).toUpper()+rec.category.mid(1);
            base_recs[1] = "";
        }

        while (base_recs.length() && base_recs.first().isEmpty()) base_recs.removeFirst();
        rec.description = base_recs.join("\n");

        records->append(rec);
        ui->functions_listWidget->addItem(rec.name);

        if (ui->category_comboBox->findText(rec.category)<0) {
            ui->category_comboBox->addItem(rec.category);
        }

        pos += rx.matchedLength();
    }

    if (ui->functions_listWidget->count()>0) {
        ui->functions_listWidget->item(0)->setSelected(true);
        on_functions_listWidget_currentRowChanged(0);
    }
}

DialogFunctions::~DialogFunctions()
{
    delete records;
    delete widget_drawer;
    delete ui;
}

QString DialogFunctions::getPickedFunction()
{
    QString result = "";
    if (last_picked_row>=0 && last_picked_row<records->size()) {
        result = records->at(last_picked_row).name;
        if (records->at(last_picked_row).function) {
            result = result.replace("(t)","(k*t)");
        }
    }
    return result;
}

void DialogFunctions::on_functions_listWidget_currentRowChanged(int currentRow)
{
    last_picked_row = currentRow;
    if (currentRow>=0 && currentRow<records->size()) {
        ui->descriptions_text->document()->setHtml("<b>"+records->at(currentRow).name+"</b><br>"+records->at(currentRow).description);
        if (records->at(currentRow).function) {
            widget_drawer->show();
            widget_drawer->setGraphicFunctionT(records->at(currentRow).function);
            widget_drawer->update();
        } else {
            widget_drawer->resetGraphicFunctions();
            widget_drawer->hide();
        }
    }
}

void DialogFunctions::on_DialogFunctions_finished(int result)
{
    if (result!=Accepted) {
        last_picked_row = -1;
    }
}

void DialogFunctions::on_functions_listWidget_activated(const QModelIndex &index)
{
    last_picked_row = index.row();
    accept();
}

void DialogFunctions::on_category_comboBox_currentIndexChanged(int index)
{
    last_picked_row = -1;
    if (records->size()>0) {
        int first_vis = -1;
        for (int i=0; i<records->size(); i++)
        {
            if (index==0 || records->at(i).category==ui->category_comboBox->itemText(index)) {
                if (first_vis<0) first_vis = i;
                ui->functions_listWidget->item(i)->setHidden(false);
            } else {
                ui->functions_listWidget->item(i)->setHidden(true);
            }
        }
        if (first_vis>=0) {
            ui->functions_listWidget->item(first_vis)->setSelected(true);
            on_functions_listWidget_currentRowChanged(first_vis);
        }
    }
}
