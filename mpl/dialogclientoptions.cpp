#include "dialogclientoptions.h"
#include "ui_dialogclientoptions.h"
#include <qjsontablemodel.h>

DialogClientOptions::DialogClientOptions(CertUser* usr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    connect(ui->chBindCertificate, SIGNAL(stateChanged(int)), this, SLOT(onChBindCertificateToggled(int)));

    _usr = usr;

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
    auto objMozilla = QJsonObject();
    objMozilla.insert("mozillaExeFile", ui->lineMozillaPath->text());
    objMozilla.insert("profilesPath", ui->lineMoxillaProfilesPath->text());
    objMozilla.insert("bindCertificates", ui->chBindCertificate->isChecked());
    objMain.insert("mozilla", objMozilla);
    auto mpl = QJsonObject();
    mpl.insert("user", ui->lineMozillaPath->text());
    mpl.insert("pwd", bWebSocket::crypt(ui->lineMoxillaProfilesPath->text(), "may_key"));
    mpl.insert("server", ui->lineMoxillaProfilesPath->text());
    mpl.insert("useSettingsFromHttp", ui->chkUseSettingsFromHttp->isChecked());
    mpl.insert("httpHost", ui->lineHttpHost->text());
    mpl.insert("httpUsr", ui->lineHttpUsr->text());
    mpl.insert("httpPwd", bWebSocket::crypt(ui->lineHttpUsr->text(), "may_key"));
    mpl.insert("launch_mode", ui->checkMode->isChecked());
    mpl.insert("ServerUser", ui->lineWsUser->text());
    mpl.insert("ServerHost", ui->lineWsServer->text());
    mpl.insert("ServerPort", ui->spinPort->value());
    QString hash = "";
    if(ui->pwdEdit->isChecked()){
        QString hash = bWebSocket::generateHash(ui->lineWsUser->text(), ui->lineWsPwd->text());
        objMozilla.insert("Hash", hash);
    }
    mpl.insert("customWsUser", ui->btnCustomWsUserEdit->isChecked());

    objMain.insert("mpl", mpl);

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

        _obj = doc.object();
        auto obj = _obj.value("mozilla").toObject();

        ui->lineMozillaPath->setText(obj.value("mozillaExeFile").toString());
        ui->lineMoxillaProfilesPath->setText(obj.value("profilesPath").toString());
        ui->chBindCertificate->setChecked(obj.value("bindCertificates").toBool());

        auto mpl = _obj.value("mpl").toObject();

        ui->lineUser->setText(mpl.value("user").toString());
        ui->lineServer->setText(mpl.value("server").toString());
        ui->linePwd->setText(bWebSocket::crypt(mpl.value("pwd").toString(), "my_key"));
        ui->chkUseSettingsFromHttp->setChecked(mpl.value("useSettingsFromHttp").toBool());
        ui->lineHttpHost->setText(mpl.value("httpHost").toString());
        ui->lineHttpUsr->setText(mpl.value("httpUsr").toString());
        QString httpPwd = mpl.value("httpPwd").toString();
        if(!httpPwd.isEmpty())
            httpPwd = bWebSocket::crypt(httpPwd, "my_key");

        ui->lineHttpPwd->setText(httpPwd);

        ui->checkMode->setChecked(mpl.value("launch_mode").toBool());

        ui->lineWsUser->setText(mpl.value("ServerUser").toString());
        ui->lineWsServer->setText(mpl.value("ServerHost").toString());
        ui->lineWsPwd->setText("***"); //mpl.value("Password").toString());
        ui->spinPort->setValue(mpl.value("ServerPort").toInt());

        ui->btnCustomWsUserEdit->setChecked(mpl.value("customWsUser").toBool());

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
