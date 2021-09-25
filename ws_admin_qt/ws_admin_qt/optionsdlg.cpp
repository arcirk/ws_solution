#include "stdfx.h"
#include "optionsdlg.h"
#include "ui_optionsdlg.h"
#include <QFileDialog>
//#include "global.h"
//#include "../../ws_client/include/global.h"
#include "../../ws_client/include/iws_client.h"

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
    ui->ServerName->setText(_settings->ServerName);
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
    _settings->ServerBinDir = ui->ServerBinDir->toPlainText();
    _settings->ServerPort = ui->ServerPort->value();
    _settings->ServerName = ui->ServerName->text();

    if(ui->editPwd->isChecked()){
        //Hash = arc_json_qt::get_hash(_settings->RootUser + ui->Password->text());
        Hash = QString::fromStdString(IClient::get_hash(_settings->RootUser.toStdString(), ui->Password->text().toStdString()).c_str());
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

