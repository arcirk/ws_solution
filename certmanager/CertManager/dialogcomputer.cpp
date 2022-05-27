#include "dialogcomputer.h"
#include "ui_dialogcomputer.h"

DialogComputer::DialogComputer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogComputer)
{
    ui->setupUi(this);
    _comp.insert("SecondField", "");
    _comp.insert("FirstField", "");
    _comp.insert("ipadrr", "");
    _comp.insert("isServer", 0);
    _comp.insert("Ref", "");
}

DialogComputer::~DialogComputer()
{
    delete ui;
}

QMap<QString, QVariant> DialogComputer::computer()
{
    return _comp;
}

void DialogComputer::setComputer(QMap<QString, QVariant> &comp)
{
    _comp = comp;
    ui->lineEdit->setText(_comp["FirstField"].toString());
    ui->lineEdit_2->setText(_comp["ipadrr"].toString());
    ui->checkBox->setChecked(_comp["isServer"].toInt());
    ui->lineEdit_3->setText(_comp["SecondField"].toString());
}

void DialogComputer::accept()
{
    if(ui->lineEdit->text().trimmed().isEmpty()){
        QMessageBox::critical(this, "Ошибка", "Введите наименование!");
        return;
    }

    QDialog::accept();
}

void DialogComputer::on_lineEdit_textChanged(const QString &arg1)
{
    _comp["FirstField"] = arg1;
}


void DialogComputer::on_lineEdit_2_textChanged(const QString &arg1)
{
    _comp["ipadrr"] = arg1;
}


void DialogComputer::on_checkBox_stateChanged(int arg1)
{
    _comp["isServer"] = arg1;
}


void DialogComputer::on_lineEdit_3_textChanged(const QString &arg1)
{
    _comp["SecondField"] = arg1;
}

