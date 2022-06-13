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
    ui->chkUseSettingsFromHttp->setChecked(_sett->useSettingsFromHttp());
    ui->lineHttpHost->setText(_sett->httpHost());
    ui->lineHttpUsr->setText(_sett->httpUsr());
    QString httpPwd = _sett->httpPwd();
    if(!_sett->httpPwd().isEmpty())
        httpPwd = bWebSocket::crypt(httpPwd, "my_key");

    ui->lineHttpPwd->setText(httpPwd);

    ui->checkMode->setChecked(_sett->launch_mode());

    //_clientSett = clientSett;

    ui->lineWsUser->setText(clientSett["ServerUser"].toString());
    ui->lineWsServer->setText(clientSett["ServerHost"].toString());
    ui->lineWsPwd->setText(clientSett["Password"].toString());
    ui->spinPort->setValue(clientSett["ServerPort"].toInt());
    ui->lineMozillaPath->setText(clientSett["mozillaExeFile"].toString());

    ui->btnCustomWsUserEdit->setChecked(_sett->customWsUser());

    formControl();
}

DialogConnection::~DialogConnection()
{
    delete ui;
}

void DialogConnection::accept()
{

    _sett->setLanchMode((launchMode)ui->checkMode->isChecked());
    _sett->setUser(ui->lineUser->text());
    _sett->setServer(ui->lineServer->text());
    _sett->setPwd(ui->linePwd->text());

    _sett->setUseSettingsFromHttp(ui->chkUseSettingsFromHttp->isChecked());

    _sett->setHttpHost(ui->lineHttpHost->text());
    _sett->setHttpUsr(ui->lineHttpUsr->text());
    QString httpPwd = ui->lineHttpPwd->text();
    if(!ui->lineHttpPwd->text().isEmpty())
        httpPwd = bWebSocket::crypt(httpPwd, "my_key");
    _sett->setHttpPwd(httpPwd);

    _clientSett["pwdEdit"] = ui->pwdEdit->isChecked();

    _clientSett["mozillaExeFile"] = ui->lineMozillaPath->text();

    _sett->setCustomWsUser(ui->btnCustomWsUserEdit->isChecked());

    if(_sett->useSettingsFromHttp()){
        if(_sett->customWsUser()){
            _clientSett["ServerUser"] = ui->lineWsUser->text();
            _clientSett["Password"] = ui->lineWsPwd->text();
        }
    }else{
        _clientSett["ServerUser"] = ui->lineWsUser->text();
        _clientSett["Password"] = ui->lineWsPwd->text();
    }

    if(!_sett->useSettingsFromHttp()){
        _clientSett["ServerHost"] = ui->lineWsServer->text();
        _clientSett["ServerPort"] = ui->spinPort->value();
    }

    QDialog::accept();

}

QString DialogConnection::pwd()
{
    return _pwd;
}

void DialogConnection::formControl()
{
    if(ui->chkUseSettingsFromHttp->isChecked()){
        ui->lineServer->setEnabled(false);
        ui->lineUser->setEnabled(false);
        ui->linePwd->setEnabled(false);
        ui->lineWsServer->setEnabled(false);
        ui->spinPort->setEnabled(false);
        ui->btnViewPwd->setEnabled(false);
        ui->pwdEdit->setEnabled(false);
        //ui->checkMode->setEnabled(false);
        ui->lineWsUser->setEnabled(false);
        if(ui->btnCustomWsUserEdit->isChecked()){
            ui->lineWsUser->setEnabled(true);
            ui->lineWsPwd->setEnabled(true);
            ui->pwdEdit->setEnabled(true);
        }else{
            ui->lineWsUser->setEnabled(false);
            ui->lineWsPwd->setEnabled(false);
            ui->pwdEdit->setEnabled(false);
        }
        ui->btnCustomWsUserEdit->setEnabled(true);
    }else{
        bool en = (bool)_sett->launch_mode();
        ui->lineServer->setEnabled(en);
        ui->lineUser->setEnabled(en);
        ui->linePwd->setEnabled(en);
        ui->lineWsServer->setEnabled(!en);
        ui->spinPort->setEnabled(!en);
        ui->btnViewPwd->setEnabled(en);
        ui->pwdEdit->setEnabled(!en);
        //ui->checkMode->setEnabled(true);
        ui->lineWsUser->setEnabled(true);
        ui->lineWsUser->setEnabled(true);
        ui->lineWsPwd->setEnabled(true);
        ui->btnCustomWsUserEdit->setChecked(false);
        ui->btnCustomWsUserEdit->setEnabled(false);
    }

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

    if(!ui->chkUseSettingsFromHttp){
        _sett->setLanchMode((launchMode)checked);
        formControl();
        if(!checked)
            ui->pwdEdit->setChecked(false);
    }
}


void DialogConnection::on_pwdEdit_toggled(bool checked)
{
    ui->btnWsPwd->setEnabled(checked);
    ui->lineWsPwd->setEnabled(checked);
}


void DialogConnection::on_chkUseSettingsFromHttp_toggled(bool checked)
{
    formControl();
}


void DialogConnection::on_btnHttpPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->lineHttpPwd->setEchoMode(echoMode);
}


void DialogConnection::on_btnCustomWsUserEdit_clicked()
{
    formControl();
}

