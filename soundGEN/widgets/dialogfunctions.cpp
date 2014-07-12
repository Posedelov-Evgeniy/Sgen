#include "dialogfunctions.h"
#include "ui_dialogfunctions.h"

DialogFunctions::DialogFunctions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFunctions)
{
    ui->setupUi(this);
    initRecords();
}

void DialogFunctions::initRecords()
{
    QString funct_str;
    QFile file(QCoreApplication::applicationDirPath()+"/base_functions.cpp");
    if(!file.exists()){
        qDebug() << "base_functions.cpp not exists";
    }
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
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
        if (base_recs.length() && base_recs.first().isEmpty()) base_recs.removeFirst();

        for(i=0; i<base_recs.length(); i++) {
            str = base_recs.at(i);
            base_recs[i] = str.replace(QRegExp("^(\\s){1,}"), "");
        }

        if (base_recs.length()>=1) {
            str = base_recs.at(0);
            rec.name = str.replace('{',' ').replace('}',' ').trimmed();
            base_recs[0] = "";
        }

        if (base_recs.length()>=2) {
            rec.category = base_recs.at(1);
            base_recs[1] = "";
        }

        rec.description = base_recs.join('\n');

        records->append(rec);

        pos += rx.matchedLength();
    }
}

DialogFunctions::~DialogFunctions()
{
    delete records;
    delete ui;
}

void DialogFunctions::on_buttonBox_clicked(QAbstractButton *button)
{
    qDebug() << records->length() << endl;
}
