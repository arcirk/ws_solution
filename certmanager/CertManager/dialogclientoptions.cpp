#include "dialogclientoptions.h"
#include "ui_dialogclientoptions.h"
#include "tabledelegate.h"
#include <QCheckBox>

DialogClientOptions::DialogClientOptions(CertUser* usr, QProxyModel* a_certs, QWidget *parent) :
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

    ui->tableAvailableCerts->setModel(a_certs);

    ui->tableAvailableCerts->resizeColumnsToContents();
    ui->tableViewProfiles->resizeColumnsToContents();

    ui->tableViewProfiles->setItemDelegate(new TableDelegate);
    ui->tableViewProfiles->setIconSize(QSize(16,16));

}

DialogClientOptions::~DialogClientOptions()
{
    delete ui;
}

void DialogClientOptions::accept()
{
    QDialog::accept();
}



void DialogClientOptions::onChBindCertificateToggled(int state)
{
    bool checked = state == Qt::Checked;

    ui->wdToolBarCerts->setEnabled(checked);
    ui->tableAvailableCerts->setEnabled(checked);
}

void DialogClientOptions::setFromUserSettings()
{
    auto cache = _usr->cache();

    _profiles = new QJsonTableModel(this);

    if(!cache.isEmpty()){

        auto _obj = cache.value("mpl").toObject();
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

        QString profilesModel = QJsonDocument(cache.value("profiles").toObject()).toJson();
        if(!profilesModel.isEmpty()){
            _profiles->setJsonText(profilesModel);
            _profiles->reset();
            ui->tableViewProfiles->setModel(_profiles);
            //ui->tableViewProfiles->resizeColumnsToContents();
        }

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
//    QString available_certs = _usr->available_certificates();
//    if(!available_certs.isNull()){

//        auto model = new QJsonTableModel(this);
//        model->setJsonText(available_certs);
//        ui->tableAvailableCerts->setModel(model);
//        ui->tableAvailableCerts->resizeColumnsToContents();

//    }

    formControl();

    updateTableImages();
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


void DialogClientOptions::on_btnCustomWsUserEdit_toggled(bool checked)
{

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
    QString hash = "";
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

void DialogClientOptions::setProfoleImage(int index, const QString &imagePath)
{
    _profiles->setIcon(_profiles->index(index, 0), QIcon(imagePath));

}

void DialogClientOptions::updateTableImages()
{
    int iLink = _profiles->getColumnIndex("address");
    for (int i = 0; i < _profiles->rowCount(); ++i) {

        QString link = _profiles->index(i, iLink).data(Qt::UserRole + iLink).toString();
        if(link.indexOf("markirovka") != -1){
            setProfoleImage(i, ":/img/markirowka.png");
        }else if(link.indexOf("diadoc.kontur.ru") != -1){
            setProfoleImage(i, ":/img/diadoc.png");
        }else if(link.indexOf("ofd.kontur.ru") != -1){
            setProfoleImage(i, ":/img/ofd.png");
        }else if(link.indexOf("extern.kontur.ru") != -1){
            setProfoleImage(i, ":/img/extern.png");
        }else
            setProfoleImage(i, ":/img/link.png");
    }

    _profiles->reset();

}
