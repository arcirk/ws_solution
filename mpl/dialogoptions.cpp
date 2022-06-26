#include "dialogoptions.h"
#include "ui_dialogoptions.h"
#include <qjsontablemodel.h>

DialogOptions::DialogOptions(ProfileManager* manager, CertUser* usr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    _manager = manager;
    _usr = usr;

    ui->lineMozillaPath->setText(_manager->mozillaExeFile());
    ui->lineMoxillaProfilesPath->setText(_manager->mozillaProfilesFile());
    ui->chBindCertificate->setChecked(_manager->bindCertificates());

    QString available_certs = _usr->available_certificates();
    if(!available_certs.isNull()){

        auto model = new QJsonTableModel(this);
        model->setJsonText(available_certs);
        ui->tableAvailableCerts->setModel(model);
        ui->tableAvailableCerts->resizeColumnsToContents();

    }


}

DialogOptions::~DialogOptions()
{
    delete ui;
}
