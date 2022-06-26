#include "dialogselectedrow.h"
#include "ui_dialogselectedrow.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStringListModel>

DialogSelectedRow::DialogSelectedRow(UserProfile * prof, CertUser* user, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectedRow)
{
    ui->setupUi(this);
    _prof = prof;

    loadProfiles();

    auto lst = new QStringListModel(m_profileNames);
    ui->cmbMozillaProfile->setModel(lst);

    if(!prof->profile().isEmpty())
        ui->cmbMozillaProfile->setCurrentIndex(m_profileNames.indexOf(prof->profile()));
    ui->lblName->setText(prof->name());
    ui->txtUrl->setText(prof->defaultAddress());

    QStringList lstCerts = {""};
    int i = 0;
    for (auto itr = user->certificates().begin(); itr != user->certificates().end(); ++itr) {
        lstCerts.append(itr.value()->bindName());
        _certsList.insert(i, itr.value());
    }

    lst = new QStringListModel(lstCerts);
    ui->cmbCertificates->setModel(lst);

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
