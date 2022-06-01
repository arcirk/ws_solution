#include "dialogconnection.h"
#include "ui_dialogconnection.h"

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

DialogConnection::DialogConnection(Settings* sett, QMap<QString, QVariant>& clientSett, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConnection)
{
    ui->setupUi(this);
    _sett = sett;

    ui->lineUser->setText(_sett->user());
    ui->lineServer->setText(_sett->server());
    ui->linePwd->setText(_sett->pwd());

    ui->checkMode->setChecked(_sett->launch_mode());

    _clientSett = clientSett;

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

}

