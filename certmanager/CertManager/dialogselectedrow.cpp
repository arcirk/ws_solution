#include "dialogselectedrow.h"
#include "ui_dialogselectedrow.h"
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStringListModel>


DialogSelectedRow::DialogSelectedRow(CertUser* user, const QJsonObject& row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectedRow)
{
    ui->setupUi(this);

    _currentObject = row;

    if(_currentObject.isEmpty()){
       _currentObject.insert("Empty", "");
       _currentObject.insert("name", "");
       _currentObject.insert("profile", "");
       _currentObject.insert("address", "");
       _currentObject.insert("uuid", "");
       _currentObject.insert("cert", "");
    }

    m_profileNames = user->mozillaProfiles();

    auto lst = new QStringListModel(m_profileNames);
    ui->cmbMozillaProfile->setModel(lst);

    if(!_currentObject.value("profile").toString().isEmpty())
        ui->cmbMozillaProfile->setCurrentIndex(m_profileNames.indexOf(_currentObject.value("profile").toString()));
    ui->lblName->setText(_currentObject.value("name").toString());
    ui->txtUrl->setText(_currentObject.value("address").toString());

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
    _currentObject["profile"] = ui->cmbMozillaProfile->currentText();
    _currentObject["address"] = ui->txtUrl->toPlainText();
    if(_currentObject["uuid"].toString().isEmpty()){
        _currentObject["uuid"] = QUuid::createUuid().toString();
    }
    _currentObject["name"] = ui->lblName->text();

    QDialog::accept();
}

QJsonObject DialogSelectedRow::resultObject()
{
    return _currentObject;
}

