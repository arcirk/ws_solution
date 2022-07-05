#include "dialogclientoptions.h"
#include "ui_dialogclientoptions.h"
#include <qjsontablemodel.h>


DialogClientOptions::DialogClientOptions(CertUser* usr, ProfileManager* mpl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    connect(ui->chBindCertificate, SIGNAL(stateChanged(int)), this, SLOT(onChBindCertificateToggled(int)));

    _usr = usr;
    _mpl = mpl;

    setFromUserSettings();

    ui->tableAvailableCerts->resizeColumnsToContents();
}

DialogClientOptions::~DialogClientOptions()
{
    delete ui;
}

QJsonObject DialogClientOptions::getOptionsCache()
{

    auto objMain = QJsonObject();
    //auto objMozilla = QJsonObject();
    objMain.insert("mozillaExeFile", ui->lineMozillaPath->text());
    objMain.insert("profilesIniFile", ui->lineMoxillaProfilesPath->text());
    objMain.insert("bindCertificates", ui->chBindCertificate->isChecked());
    objMain.insert("user", ui->lineUser->text());
    objMain.insert("pwd", bWebSocket::crypt(ui->linePwd->text(), "my_key"));
    objMain.insert("server", ui->lineServer->text());
    objMain.insert("useSettingsFromHttp", ui->chkUseSettingsFromHttp->isChecked());
    objMain.insert("httpHost", ui->lineHttpHost->text());
    objMain.insert("httpUsr", ui->lineHttpUsr->text());
    objMain.insert("httpPwd", bWebSocket::crypt(ui->lineHttpPwd->text(), "my_key"));
    objMain.insert("launch_mode", ui->checkMode->isChecked());
    objMain.insert("ServerUser", ui->lineWsUser->text());
    objMain.insert("ServerHost", ui->lineWsServer->text());
    objMain.insert("ServerPort", ui->spinPort->value());
    if(ui->pwdEdit->isChecked()){
        QString hash = bWebSocket::generateHash(ui->lineWsUser->text(), ui->lineWsPwd->text());
        objMain.insert("Hash", hash);
    }else
        objMain.insert("Hash", _hash);
    objMain.insert("customWsUser", ui->btnCustomWsUserEdit->isChecked());

    objMain.insert("charset", ch.isEmpty() ? "CP866": ch);
    objMain.insert("method", method);

    return objMain;

}

void DialogClientOptions::onChBindCertificateToggled(int state)
{
    bool checked = state == Qt::Checked;

    ui->tableAvailableCerts->setEnabled(checked);
}

void DialogClientOptions::setFromUserSettings()
{
    //QString cache = _usr->cache();
    auto doc = QJsonDocument(_usr->cache());

    if(!doc.isEmpty()){

        _obj = _mpl->to_object().value("mpl").toObject();
        ch = _obj.value("charset").toString();
        method = _obj.value("method").toInt();
        _hash = _obj.value("Hash").toString();

        ui->lineMozillaPath->setText(_obj.value("mozillaExeFile").toString());
        ui->lineMoxillaProfilesPath->setText(_obj.value("profilesIniFile").toString());
        ui->chBindCertificate->setChecked(_obj.value("bindCertificates").toBool());

        ui->lineUser->setText(_obj.value("user").toString());
        ui->lineServer->setText(_obj.value("server").toString());
        ui->linePwd->setText(bWebSocket::crypt(_obj.value("pwd").toString(), "my_key"));
        ui->chkUseSettingsFromHttp->setChecked(_obj.value("useSettingsFromHttp").toBool());
        ui->lineHttpHost->setText(_obj.value("httpHost").toString());
        ui->lineHttpUsr->setText(_obj.value("httpUsr").toString());
        QString httpPwd = _obj.value("httpPwd").toString();
        if(!httpPwd.isEmpty())
            httpPwd = bWebSocket::crypt(httpPwd, "my_key");

        ui->lineHttpPwd->setText(httpPwd);

        ui->checkMode->setChecked(_obj.value("launch_mode").toBool());

        ui->lineWsUser->setText(_obj.value("ServerUser").toString());
        ui->lineWsServer->setText(_obj.value("ServerHost").toString());
        ui->lineWsPwd->setText("***"); //mpl.value("Password").toString());
        ui->spinPort->setValue(_obj.value("ServerPort").toInt());

        ui->btnCustomWsUserEdit->setChecked(_obj.value("customWsUser").toBool());

        emit ui->chBindCertificate->stateChanged(ui->chBindCertificate->checkState());
    }else{

    }
    QString available_certs = _usr->available_certificates();
    if(!available_certs.isNull()){

        auto model = new QJsonTableModel(this);
        model->setJsonText(available_certs);
        ui->tableAvailableCerts->setModel(model);
        ui->tableAvailableCerts->resizeColumnsToContents();

    }

    formControl();
}

void DialogClientOptions::formControl()
{
    bool fromHttp = ui->chkUseSettingsFromHttp->isChecked();
    if(fromHttp){
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
        bool en = ui->checkMode->isChecked();
        ui->lineServer->setEnabled(en);
        ui->lineUser->setEnabled(en);
        ui->linePwd->setEnabled(en);
        ui->lineWsServer->setEnabled(!en);
        ui->spinPort->setEnabled(!en);
        ui->btnViewPwd->setEnabled(en);
        ui->pwdEdit->setEnabled(true);
        ui->lineWsUser->setEnabled(true);
        ui->lineWsPwd->setEnabled(true);
        ui->btnCustomWsUserEdit->setChecked(false);
        ui->btnCustomWsUserEdit->setEnabled(false);
    }

}

void DialogClientOptions::on_btnHttpPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->lineHttpPwd->setEchoMode(echoMode);
}


void DialogClientOptions::on_btnWsPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->lineWsPwd->setEchoMode(echoMode);
}


void DialogClientOptions::on_btnCustomWsUserEdit_clicked()
{
    formControl();
}


void DialogClientOptions::on_btnViewPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString image = checked ? ":/img/viewPwd.svg" : ":/img/viewPwd1.svg";
    auto btn = dynamic_cast<QToolButton*>(sender());
    btn->setIcon(QIcon(image));
    ui->linePwd->setEchoMode(echoMode);
}


void DialogClientOptions::on_checkMode_toggled(bool checked)
{
    if(!ui->chkUseSettingsFromHttp){
        if(!checked)
            ui->pwdEdit->setChecked(false);
    }

    formControl();
}


void DialogClientOptions::on_pwdEdit_clicked()
{

}


void DialogClientOptions::on_pwdEdit_toggled(bool checked)
{
    ui->btnWsPwd->setEnabled(checked);
    ui->lineWsPwd->setEnabled(checked);
}


void DialogClientOptions::on_btnCustomWsUserEdit_toggled(bool checked)
{

}


void DialogClientOptions::on_linePwd_textChanged(const QString &arg1)
{

}

