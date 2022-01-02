#include "../include/optionsdialog.h"
#include "../form/ui_optionsdialog.h"

#include <QFileDialog>

OptionsDialog::OptionsDialog(QWidget *parent, const ClientSettings& sett) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    ui->chkSrvLocalInstall->setChecked(sett.isLocalInstallation);
    ui->chkRunAsService->setChecked(sett.RunAsService);
    ui->txtWorkingDirectory->setText(sett.ServerWorkingDirectory);
    ui->ServerName->setText(sett.ServerName);
    ui->ServerHost->setText(sett.ServerHost);
    ui->ServerPort->setValue(sett.ServerPort);
    ui->RootUser->setText(sett.RootUser);
    ui->ServerName->setText(sett.ServerName);
    ui->chkAutoConnect->setChecked(sett.AutoConnect);
    ui->chkSavePwd->setChecked(sett.SaveHash);
    ui->Password->setText("***");

    ui->radioUseLocalFolder->setChecked(sett.UseLocalWebDAvDirectory);
    ui->txtLocalFilesFolder->setText(sett.LocalWebDavDirectory);
    ui->lineAddressWebDav->setText(sett.WebdavHost);
    ui->lineEdtWebDAvUser->setText(sett.WebdavUser);
    ui->lineEdtWebDavPassword->setText(sett.WebdavPwd);
    ui->chSSL->setChecked(sett.WebdavSSL);

    settings = sett;
    settings.password = "***";
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

ClientSettings& OptionsDialog::getSettings() const
{
    return const_cast<ClientSettings &>(settings);
}

void OptionsDialog::on_chkSrvLocalInstall_toggled(bool checked)
{
    settings.isLocalInstallation = checked;
}


void OptionsDialog::on_chkRunAsService_toggled(bool checked)
{
    settings.RunAsService = checked;
}


void OptionsDialog::on_txtWorkingDirectory_textChanged()
{
    settings.ServerWorkingDirectory = ui->txtWorkingDirectory->toPlainText();
}


void OptionsDialog::on_OptionsDialog_accepted()
{
   //std::copy()
}


void OptionsDialog::on_btnSelectFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->txtWorkingDirectory->setText(dir);
    }

}


void OptionsDialog::on_ServerName_editingFinished()
{
    settings.ServerName = ui->ServerName->text();
}


void OptionsDialog::on_ServerHost_editingFinished()
{
    settings.ServerHost = ui->ServerHost->text();
}


void OptionsDialog::on_ServerPort_valueChanged(double arg1)
{
    settings.ServerPort = arg1;
}


void OptionsDialog::on_RootUser_editingFinished()
{
    settings.RootUser = ui->RootUser->text();
}


void OptionsDialog::on_btnViewPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    ui->Password->setEchoMode(echoMode);
}


void OptionsDialog::on_editPwd_toggled(bool checked)
{
    //QString defPwd = checked ? "" : ;
    ui->Password->setEnabled(checked);
    //ui->Password->setText(defPwd);
    ui->btnViewPwd->setEnabled(checked);
    settings.pwdEdit = checked;
}


void OptionsDialog::on_Password_editingFinished()
{
    settings.password = ui->Password->text();
}


void OptionsDialog::on_chkSavePwd_toggled(bool checked)
{
    settings.SaveHash = checked;
}


void OptionsDialog::on_chkAutoConnect_toggled(bool checked)
{
    settings.AutoConnect = checked;
}


void OptionsDialog::on_radioUseLocalFolder_toggled(bool checked)
{
    settings.UseLocalWebDAvDirectory = checked;
}


void OptionsDialog::on_txtLocalFilesFolder_textChanged()
{
    settings.LocalWebDavDirectory = ui->txtLocalFilesFolder->toPlainText();
}


void OptionsDialog::on_lineAddressWebDav_editingFinished()
{
    settings.WebdavHost = ui->lineAddressWebDav->text();
}


void OptionsDialog::on_chSSL_toggled(bool checked)
{
    settings.WebdavSSL = checked;
}


void OptionsDialog::on_lineEdtWebDAvUser_editingFinished()
{
    settings.WebdavUser = ui->lineEdtWebDAvUser->text();
}


void OptionsDialog::on_lineEdtWebDavPassword_editingFinished()
{
    settings.WebdavPwd = ui->lineEdtWebDavPassword->text();
}

