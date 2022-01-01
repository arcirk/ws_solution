//#include "stdfx.h"
#include "optionsdlg.h"
#include "ui_optionsdlg.h"
#include <QFileDialog>
#include <QDir>

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
    ui->radioUseWebDav->setChecked(_settings->useWebDav);
    if(!_settings->useWebDav){
       ui->radioUseLocalFolder->setChecked(true);
    }
    ui->lineAddressWebDav->setText(_settings->webDavAddress);
    ui->lineEdtWebDAvUser->setText(_settings->davUser);
    ui->lineEdtWebDavPassword->setText(_settings->davPassword);
    ui->txtLocalFilesFolder->setText(_settings->localFolder);

    Hash = "";

}

OptionsDlg::~OptionsDlg()
{
    delete ui;
}

void OptionsDlg::on_btnSelectDir_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
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
    _settings->useWebDav = ui->radioUseWebDav->isChecked();
    _settings->davUser = ui->lineEdtWebDAvUser->text();
    _settings->davPassword = ui->lineEdtWebDavPassword->text();
    _settings->localFolder = ui->txtLocalFilesFolder->toPlainText();
    _settings->webDavAddress = ui->lineAddressWebDav->text();

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


void OptionsDlg::on_btnDavPwd_toggled(bool checked)
{
    if(checked){
        ui->lineEdtWebDavPassword->setEchoMode(QLineEdit::Normal);
        ui->btnDavPwd->setIcon(QIcon(QStringLiteral(":/new/icons/res/viewPwd.svg")));
    }else{
        ui->lineEdtWebDavPassword->setEchoMode(QLineEdit::Password);
        ui->btnDavPwd->setIcon(QIcon(QStringLiteral(":/new/icons/res/viewPwd1.svg")));
    }
}


void OptionsDlg::on_radioUseWebDav_toggled(bool checked)
{
    ui->radioUseLocalFolder->setChecked(!checked);
}


void OptionsDlg::on_radioUseLocalFolder_toggled(bool checked)
{
    ui->radioUseWebDav->setChecked(!checked);
}


void OptionsDlg::on_btnSelectLocalFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->txtLocalFilesFolder->setText(dir);
    }
}


void OptionsDlg::on_lineAddressWebDav_editingFinished()
{

}


void OptionsDlg::on_ServerBinDir_textChanged()
{

}

