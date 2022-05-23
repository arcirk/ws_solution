#include "dialogoptions.h"
#include "ui_dialogoptions.h"
#include <clientsettings.h>

DialogOptions::DialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);
    auto opt = ClientSettings("conf_qt_cert_manager.json");
    ui->lineSqlHost->setText(opt[bConfFieldsWrapper::SQLHost].toString());
    ui->lineSqlUser->setText(opt[bConfFieldsWrapper::SQLUser].toString());
    ui->lineSqlPassword->setText(opt[bConfFieldsWrapper::SQLPassword].toString());
    ui->lineWsHost->setText(opt[bConfFieldsWrapper::ServerHost].toString());
    ui->spinWsPort->setValue(opt[bConfFieldsWrapper::ServerPort].toInt());
    ui->lineWsUser->setText(opt[bConfFieldsWrapper::User].toString());
    ui->lineWsPassword->setText(opt[bConfFieldsWrapper::Hash].toString());
}

DialogOptions::~DialogOptions()
{
    delete ui;
}
