#include "dialogconnection.h"
#include "ui_dialogconnection.h"

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

DialogConnection::DialogConnection(settings* sett, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnection)
{
    ui->setupUi(this);
    _sett = sett;

    ui->lineUser->setText(_sett->user());
    ui->lineServer->setText(_sett->server());
    ui->linePwd->setText(_sett->pwd());
    ui->checkMode->setChecked(_sett->launch_mode());

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
    QDialog::accept();

}

QString DialogConnection::pwd()
{
    return _pwd;
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


void DialogConnection::on_checkMode_stateChanged(int arg1)
{
    int val = arg1 >= 1 ? 1 : 0;
    bool en = val > 0;

    _sett->setLanchMode((launchMode)en);
    ui->lineServer->setEnabled(en);
    ui->lineUser->setEnabled(en);
    ui->linePwd->setEnabled(en);
}

