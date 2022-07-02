#include "dialogselectedrow.h"
#include "ui_dialogselectedrow.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStringListModel>
#include <certificate.h>
#include <QUuid>

DialogSelectedRow::DialogSelectedRow(UserProfile * prof, CertUser* user, bool bindCert, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectedRow)
{
    ui->setupUi(this);
    _prof = prof;
    _bindCert = bindCert;
    _user = user;

    loadProfiles();

    auto lst = new QStringListModel(m_profileNames);
    ui->cmbMozillaProfile->setModel(lst);

    if(!prof->profile().isEmpty())
        ui->cmbMozillaProfile->setCurrentIndex(m_profileNames.indexOf(prof->profile()));
    ui->lblName->setText(prof->name());
    ui->txtUrl->setText(prof->defaultAddress());

    QStringList lstCerts = {""};
    int defIndex = -1;
    if(!bindCert){
        int i = 0;
        for (auto itr = user->certificates().begin(); itr != user->certificates().end(); ++itr) {
            lstCerts.append(itr.value()->bindName());
            if(itr.value()->ref() == prof->certsUuidToString())
                defIndex = i;
            _certsList.insert(i, itr.value());
            i++;
        }
    }else{
        QString jAvaiableCerts = user->available_certificates();
        auto certs = QJsonDocument::fromJson(jAvaiableCerts.toUtf8()).object().value("rows").toArray();

        for (int i = 0; i < certs.size(); ++i) {
            QString val = certs[i].toObject().value("FirstField").toString();
            lstCerts.append(val);
            if(prof->certsUuidToString().indexOf(certs[i].toObject().value("CertRef").toString()) != -1)
                defIndex = lstCerts.size() -1;
        }
    }

    lst = new QStringListModel(lstCerts);
    ui->cmbCertificates->setModel(lst);
    if(defIndex != -1)
        ui->cmbCertificates->setCurrentIndex(defIndex);

    setWindowTitle("Настройка профиля");
}

DialogSelectedRow::~DialogSelectedRow()
{
    delete ui;
}

void DialogSelectedRow::accept()
{
    _prof->setProfile(ui->cmbMozillaProfile->currentText());
    _prof->setDefaultAddress(ui->txtUrl->toPlainText());
    if(_prof->isNew())
        _prof->setUuid(QUuid::createUuid());
    _prof->setName(ui->lblName->text());
    _prof->cerificates().clear();
    if(!ui->cmbCertificates->currentText().isEmpty()){
        if(!_bindCert){
            for (auto itr = _user->certificates().begin(); itr != _user->certificates().end(); ++itr) {
                if(itr.value()->bindName() == ui->cmbCertificates->currentText()){
                    _prof->cerificates().append(QUuid::fromString(itr.value()->ref()));
                    break;
                }
            }
        }else{
            QString jAvaiableCerts = _user->available_certificates();
            auto certs = QJsonDocument::fromJson(jAvaiableCerts.toUtf8()).object().value("rows").toArray();
//            foreach(auto itr, certs){
//                 auto obj = itr.toObject();
//                 if(obj.value("FirstField").toString() == ui->cmbCertificates->currentText()){
//                     auto cert = new Certificate();
//                     cert->setRef(obj.value("CertRef").toString());
//                     _prof->cerificates().append(QUuid::fromString(cert->ref()));
//                 }
//            }
            auto obj = certs[ui->cmbCertificates->currentIndex() - 1].toObject();
            auto cert = new Certificate();
            cert->setRef(obj.value("CertRef").toString());
            _prof->cerificates().append(QUuid::fromString(cert->ref()));
        }
    }
    QDialog::accept();
}

void DialogSelectedRow::loadProfiles()
{
    QString appData = QDir::homePath();

#ifndef Q_OS_LINUX
    appData.append("/AppData/Roaming/Mozilla/Firefox/profiles.ini");
#else
    appData.append("/.mozilla/firefox/profiles.ini");
#endif

    qDebug() << QDir::fromNativeSeparators(appData);

    if(!QFile::exists(QDir::fromNativeSeparators(appData))){
#ifdef Q_OS_LINUX
        if(QFile::exists(QDir::homePath() + QDir::separator() + "snap")){
            appData = QDir::homePath()  + "/snap/firefox/common/.mozilla/firefox/profiles.ini";
        }
        if(!QFile::exists(appData))
            return;
#else
        return;
#endif
    }

    QSettings ini = QSettings(QDir::fromNativeSeparators(appData), QSettings::IniFormat);
    QStringList keys = ini.allKeys();
    foreach(const QString& key, keys){
        if(key.compare("Profile")){
            if(key.endsWith("/Name")){
                m_profileNames.append(ini.value(key).toString());
            }

        }
    }
}
