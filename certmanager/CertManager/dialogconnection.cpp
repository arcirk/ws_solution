#include "dialogconnection.h"
#include "ui_dialogconnection.h"

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

DialogConnection::DialogConnection(Settings* sett, QMap<QString, QVariant>& clientSett, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnection),
    _clientSett(clientSett)
{
    ui->setupUi(this);
    _sett = sett;

    ui->lineUser->setText(_sett->user());
    ui->lineServer->setText(_sett->server());
    ui->linePwd->setText(_sett->pwd());

    ui->checkMode->setChecked(_sett->launch_mode());

    //_clientSett = clientSett;

    ui->lineWsUser->setText(clientSett["ServerUser"].toString());
    ui->lineWsServer->setText(clientSett["ServerHost"].toString());
    ui->lineWsPwd->setText(clientSett["Password"].toString());
    ui->spinPort->setValue(clientSett["ServerPort"].toInt());

    formControl();
}

DialogConnection::~DialogConnection()
{
    delete ui;
}

void DialogConnection::accept()
{

    _sett->setUser(ui->lineUser->text());
    _sett->setServer(ui->lineServer->text());
    _sett->setPwd(ui->linePwd->text());

    _clientSett["ServerUser"] = ui->lineWsUser->text();
    _clientSett["Password"] = ui->lineWsPwd->text();
    _clientSett["ServerHost"] = ui->lineWsServer->text();
    _clientSett["ServerPort"] = ui->spinPort->value();

    _clientSett["pwdEdit"] = ui->pwdEdit->isChecked();

    QDialog::accept();

}

QString DialogConnection::pwd()
{
    return _pwd;
}

void DialogConnection::formControl()
{
    bool en = (bool)_sett->launch_mode();
    ui->lineServer->setEnabled(en);
    ui->lineUser->setEnabled(en);
    ui->linePwd->setEnabled(en);
    ui->lineWsServer->setEnabled(!en);
    ui->spinPort->setEnabled(!en);
    ui->btnViewPwd->setEnabled(en);
    ui->pwdEdit->setEnabled(!en);

}

void DialogConnection::on_lineServer_textChanged(const QString &arg1)
{
    //_sett->setServer()
}


void DialogConnection::on_btnViewPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->linePwd->setEchoMode(echoMode);

}


void DialogConnection::on_linePwd_textChanged(const QString &arg1)
{
    _pwd = arg1;
}


void DialogConnection::on_btnWsPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->lineWsPwd->setEchoMode(echoMode);
}


void DialogConnection::on_checkMode_toggled(bool checked)
{

    _sett->setLanchMode((launchMode)checked);
    formControl();
    if(!checked)
        ui->pwdEdit->setChecked(false);

}


void DialogConnection::on_pwdEdit_toggled(bool checked)
{
    ui->btnWsPwd->setEnabled(checked);
    ui->lineWsPwd->setEnabled(checked);
}

