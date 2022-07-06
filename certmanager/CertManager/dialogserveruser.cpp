#include "dialogserveruser.h"
#include "ui_dialogserveruser.h"
#include <qmlwebsocket.h>

DialogServerUser::DialogServerUser(const QJsonObject& object, const QString& parentName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogServerUser)
{
    ui->setupUi(this);

    _obj = object;

    ui->lblGroup->setText(parentName);
    ui->lblName->setText(object.value("FirstField").toString());
    ui->lblPresent->setText(object.value("SecondField").toString());
    ui->lblPwd->setText("***");
}

DialogServerUser::~DialogServerUser()
{
    delete ui;
}

void DialogServerUser::accept()
{
    _obj["FirstField"] = ui->lblName->text();
    _obj["SecondField"] = ui->lblPresent->text();

    QDialog::accept();
}

QJsonObject DialogServerUser::resultObject() const
{
    return _obj;
}

QString DialogServerUser::hash()
{
    if(ui->btnPwdEdit->isChecked()){
        return bWebSocket::generateHash(ui->lblName->text(), ui->lblPwd->text());
    }else
        return "";
}

void DialogServerUser::on_btnPwdEdit_clicked()
{



}


void DialogServerUser::on_btnPwdEdit_toggled(bool checked)
{
    if(checked){
        if(ui->lblPwd->text() == "***"){
            ui->lblPwd->setText("");
        }
    }else{
        if(ui->lblPwd->text() == ""){
            ui->lblPwd->setText("***");
        }
    }

    ui->lblPwd->setEnabled(checked);
}


void DialogServerUser::on_btnPwdView_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->lblPwd->setEchoMode(echoMode);
}

