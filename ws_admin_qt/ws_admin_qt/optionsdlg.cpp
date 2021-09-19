#include "optionsdlg.h"
#include "ui_optionsdlg.h"
#include <QFileDialog>
#include "global.h"

OptionsDlg::OptionsDlg(appSettings * settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDlg)
{
    _settings = settings;

    ui->setupUi(this);

    ui->ServerHost->setText(_settings->ServerHost);
    ui->ServerPort->setValue(_settings->ServerPort);
    ui->ServerBinDir->setText(_settings->ServerBinDir);
    ui->RootUser->setText(_settings->RootUser);
    ui->Password->setText(_settings->Hash);
    Hash = "";

}

OptionsDlg::~OptionsDlg()
{
    delete ui;
}

void OptionsDlg::on_btnSelectDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->ServerBinDir->setText(dir);
    }

}


void OptionsDlg::on_buttonBox_accepted()
{
    _settings->RootUser = ui->RootUser->text();
    _settings->ServerHost = ui->ServerHost->text();
    _settings->ServerBinDir = ui->ServerBinDir->text();
    _settings->ServerPort = ui->ServerPort->value();
    if(ui->editPwd->isChecked()){
        Hash = arc_json::get_hash(_settings->RootUser + ui->Password->text());
        _settings->Hash = Hash;
    }

}


void OptionsDlg::on_editPwd_clicked(bool checked)
{
    ui->Password->setText("");
    ui->Password->setEnabled(checked);
    if(!checked){
        ui->Password->setText(_settings->Hash);
    }
}

