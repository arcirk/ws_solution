#include "dialogclientoptions.h"
#include "ui_dialogclientoptions.h"
#include <qjsontablemodel.h>
#include <QCheckBox>

DialogClientOptions::DialogClientOptions(CertUser* usr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    connect(ui->chBindCertificate, SIGNAL(stateChanged(int)), this, SLOT(onChBindCertificateToggled(int)));

    _usr = usr;

    setFromUserSettings();

//    ui->lineMozillaPath->setText(_manager->mozillaExeFile());
//    ui->lineMoxillaProfilesPath->setText(_manager->mozillaProfilesFile());
//    ui->chBindCertificate->setChecked(_manager->bindCertificates());


    ui->tableAvailableCerts->resizeColumnsToContents();
    ui->tableViewProfiles->resizeColumnsToContents();


}

DialogClientOptions::~DialogClientOptions()
{
    delete ui;
}



void DialogClientOptions::onChBindCertificateToggled(int state)
{
    bool checked = state == Qt::Checked;

    ui->wdToolBarCerts->setEnabled(checked);
    ui->tableAvailableCerts->setEnabled(checked);
}

void DialogClientOptions::setFromUserSettings()
{
    QString cache = _usr->cache();

    auto _profiles = new QJsonTableModel(this);

    if(!cache.isEmpty()){
        auto doc = QJsonDocument::fromJson(cache.toUtf8());
        _obj = doc.object();
        auto obj = _obj.value("mozilla").toObject();

        ui->lineMozillaPath->setText(obj.value("mozillaExeFile").toString());
        ui->lineMoxillaProfilesPath->setText(obj.value("profilesPath").toString());
        ui->chBindCertificate->setChecked(obj.value("bindCertificates").toBool());

        auto rows = _obj.value("profiles").toArray();

        auto _model = QJsonObject();
        auto columns = QJsonArray();
        columns.append("address");
        columns.append("certs");
        columns.append("name");
        columns.append("profile");
        columns.append("uuid");
        _model.insert("columns", columns);
        _model.insert("rows", rows);
        _profiles->setJsonText(QJsonDocument(_model).toJson());
        ui->tableViewProfiles->setModel(_profiles);
        _profiles->reset();

        auto mpl = _obj.value("mpl").toObject();

        ui->lineUser->setText(mpl.value("user").toString());
        ui->lineServer->setText(mpl.value("server").toString());
        ui->linePwd->setText(mpl.value("pwd").toString());
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

        ui->btnCustomWsUserEdit->setChecked(mpl.value("server").toBool());

        emit ui->chBindCertificate->stateChanged(ui->chBindCertificate->checkState());
    }else{
        auto _model = QJsonObject();
        auto columns = QJsonArray();
        columns.append("address");
        columns.append("certs");
        columns.append("name");
        columns.append("profile");
        columns.append("uuid");
        _model.insert("columns", columns);
        _model.insert("rows", QJsonArray());
        _profiles->setJsonText(QJsonDocument(_model).toJson());
        ui->tableViewProfiles->setModel(_profiles);
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
