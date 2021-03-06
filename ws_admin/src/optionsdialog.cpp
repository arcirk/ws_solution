#include "../include/optionsdialog.h"

#ifndef USE_QT_CREATOR
#include "../form/ui_optionsdialog.h"
#else
#include "ui_optionsdialog.h"
#endif
#include <iws_client.h>
#include <QFileDialog>
#include <QMessageBox>
#include <bwebdav.h>
#include <QStringListModel>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    settings("conf_admin_console.json", false)
{
    ui->setupUi(this);

    qDebug() << settings[bConfFieldsWrapper::SQLFormat].toString();

    ui->chkSrvLocalInstall->setChecked(settings[bConfFieldsWrapper::isLocalInstallation].toBool());
    ui->chkRunAsService->setChecked(settings[bConfFieldsWrapper::RunAsService].toBool());
    ui->txtWorkingDirectory->setText(settings[bConfFieldsWrapper::ServerWorkingDirectory].toString());
    ui->ServerName->setText(settings[bConfFieldsWrapper::ServerName].toString());
    ui->ServerHost->setText(settings[bConfFieldsWrapper::ServerHost].toString());
    ui->ServerPort->setValue(settings[bConfFieldsWrapper::ServerPort].toInt());
    ui->RootUser->setText(settings[bConfFieldsWrapper::User].toString());
    ui->chkAutoConnect->setChecked(settings[bConfFieldsWrapper::AutoConnect].toBool());
    ui->chkSavePwd->setChecked(settings[bConfFieldsWrapper::SaveHash].toBool());
    ui->Password->setText("***");

    ui->radioUseLocalFolder->setChecked(settings[bConfFieldsWrapper::UseLocalWebDavDirectory].toBool());
    ui->txtLocalFilesFolder->setText(settings[bConfFieldsWrapper::LocalWebDavDirectory].toString());
    ui->lineAddressWebDav->setText(settings[bConfFieldsWrapper::WebDavHost].toString());
    ui->lineEdtWebDAvUser->setText(settings[bConfFieldsWrapper::WebDavUser].toString());

    std::string dPwd = settings[bConfFieldsWrapper::WebDavPwd].toString().toStdString();
    QString pass;
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    ui->lineEdtWebDavPassword->setText(pass);
    ui->chSSL->setChecked(settings[bConfFieldsWrapper::WebDavSSL].toBool());

    QStringList lstFormat = {"SQLITE", "SQLSERVER"};
    QStringListModel * lst = new QStringListModel(lstFormat);
    ui->cmbFormatDb->setModel(lst);

    QString sqlFormat = settings[bConfFieldsWrapper::SQLFormat].toString();
    if(sqlFormat.isEmpty())
        sqlFormat = "SQLITE";

    int index = (sqlFormat == "SQLITE") ? 0 : 1;
    ui->cmbFormatDb->setCurrentIndex(index);

    ui->lineSqlHost->setText(settings[bConfFieldsWrapper::SQLHost].toString());
    ui->lineSqlUser->setText(settings[bConfFieldsWrapper::SQLUser].toString());
    if(ui->lineSqlUser->text().isEmpty()){
       ui->lineSqlUser->setText("sa");
       settings[bConfFieldsWrapper::SQLUser] = "sa";
    }

    dPwd = settings[bConfFieldsWrapper::SQLPassword].toString().toStdString();
    pass = "";
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    ui->lineSqlPassword->setText(pass);

    ui->lineHsHost->setText(settings[bConfFieldsWrapper::HSHost].toString());
    ui->lineHsUser->setText(settings[bConfFieldsWrapper::HSUser].toString());
    dPwd = settings[bConfFieldsWrapper::HSPassword].toString().toStdString();
    pass = "";
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    ui->lineHsPassword->setText(pass);

    settings.password = "***";


    qWebdav= new QWebdav(this, settings.confFileName());
    QObject::connect(qWebdav, SIGNAL(errorChanged(QNetworkReply::NetworkError, QString)), this, SLOT(onWebDavError(QNetworkReply::NetworkError, QString)));


}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::on_chkSrvLocalInstall_toggled(bool checked)
{
    settings[bConfFieldsWrapper::isLocalInstallation] = checked;
}

void OptionsDialog::on_chkRunAsService_toggled(bool checked)
{
    settings[bConfFieldsWrapper::RunAsService] = checked;
}

void OptionsDialog::on_txtWorkingDirectory_textChanged()
{
    settings[bConfFieldsWrapper::ServerWorkingDirectory] = ui->txtWorkingDirectory->toPlainText();
}

void OptionsDialog::on_OptionsDialog_accepted()
{
   if(settings.pwdEdit){
       settings[bConfFieldsWrapper::Hash] = QString::fromStdString(IClient::get_hash(settings[bConfFieldsWrapper::User].toString().toStdString(), settings.password.toStdString()));
    }
    settings.pwdEdit = false;
    settings.password = "";

    QString pass;
    std::string dPwd = ui->lineEdtWebDavPassword->text().toStdString();
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    settings[bConfFieldsWrapper::WebDavPwd] = pass;

    pass = "";
     dPwd = ui->lineSqlPassword->text().toStdString();
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    settings[bConfFieldsWrapper::SQLPassword] = pass;
    settings[bConfFieldsWrapper::SQLFormat] = ui->cmbFormatDb->currentText();

    pass = "";
     dPwd = ui->lineHsPassword->text().toStdString();
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    settings[bConfFieldsWrapper::HSPassword] = pass;

    settings.save();
}

void OptionsDialog::on_btnSelectFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("?????????????? ??????????????"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->txtWorkingDirectory->setText(dir);
    }

}

void OptionsDialog::on_ServerName_editingFinished()
{
    settings[bConfFieldsWrapper::ServerName] = ui->ServerName->text();
}

void OptionsDialog::on_ServerHost_editingFinished()
{
    settings[bConfFieldsWrapper::ServerHost] = ui->ServerHost->text();
}

void OptionsDialog::on_ServerPort_valueChanged(double arg1)
{
    settings[bConfFieldsWrapper::ServerPort] = (int)arg1;
}

void OptionsDialog::on_RootUser_editingFinished()
{
    settings[bConfFieldsWrapper::User] = ui->RootUser->text();
}

void OptionsDialog::on_btnViewPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    ui->Password->setEchoMode(echoMode);
}

void OptionsDialog::on_editPwd_toggled(bool checked)
{
    ui->Password->setEnabled(checked);
    ui->btnViewPwd->setEnabled(checked);
    settings.pwdEdit = checked;
}

void OptionsDialog::on_Password_editingFinished()
{
    settings.password = ui->Password->text();
}

void OptionsDialog::on_chkSavePwd_toggled(bool checked)
{
    settings[bConfFieldsWrapper::SaveHash] = checked;
}

void OptionsDialog::on_chkAutoConnect_toggled(bool checked)
{
    settings[bConfFieldsWrapper::AutoConnect] = checked;
}

void OptionsDialog::on_radioUseLocalFolder_toggled(bool checked)
{
    settings[bConfFieldsWrapper::UseLocalWebDavDirectory] = checked;
}

void OptionsDialog::on_txtLocalFilesFolder_textChanged()
{
    settings[bConfFieldsWrapper::LocalWebDavDirectory] = ui->txtLocalFilesFolder->toPlainText();
}

void OptionsDialog::on_lineAddressWebDav_editingFinished()
{
    settings[bConfFieldsWrapper::WebDavHost] = ui->lineAddressWebDav->text();
}

void OptionsDialog::on_chSSL_toggled(bool checked)
{
    settings[bConfFieldsWrapper::WebDavSSL] = checked;
}

void OptionsDialog::on_lineEdtWebDAvUser_editingFinished()
{
    settings[bConfFieldsWrapper::WebDavUser] = ui->lineEdtWebDAvUser->text();
}

void OptionsDialog::on_lineEdtWebDavPassword_editingFinished()
{
    //settings[bConfFieldsWrapper::WebDavPwd] = ui->lineEdtWebDavPassword->text();
}

void OptionsDialog::on_btnViewDavPwd_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    ui->lineEdtWebDavPassword->setEchoMode(echoMode);
}

void OptionsDialog::on_btnVerifyWebDav_clicked()
{

    //???????????? ???? ??????????????
    qWebdav->setUser(ui->lineEdtWebDAvUser->text());
    qWebdav->setPassword(ui->lineEdtWebDavPassword->text());
    qWebdav->setHost(ui->lineAddressWebDav->text());
    qWebdav->setSsl(ui->chSSL->isChecked());

    //???????????????????? ????????????
    bool isConnections = qWebdav->isConnections();
    bool exists = false;
    if(isConnections)
        exists = qWebdav->rootExists();

    if (!isConnections){
        QMessageBox::critical(this, "????????????", "???????????? ????????????????????!");
    }else{
        if (exists){
            QMessageBox::information(this, tr("??????????"),
                                     tr("???????????????? ??????????????????????!"));
        }else{
            auto result =  QMessageBox::question(this, "?????????????? ???? ??????????????", "???????????? ????????, ???? ???????????????? ?????????????? ?????????????????? ???? ????????????.\n"
                                                                             "?????????????? ???????????????");
            if(result == QMessageBox::Yes){
                bool mkdir = qWebdav->mkdirSynch("");
                if(mkdir)
                    QMessageBox::information(this, tr("??????????"),
                                             tr("?????????????? ?????????????? ????????????!"));
                else
                    QMessageBox::critical(this, tr("????????????"),
                                             tr("???????????? ???????????????? ????????????????!"));
            }
        }
    }

    //?????????????????????? ???????????? ??????????????????
    qWebdav->resetSettings();
}


void OptionsDialog::onWebDavError(QNetworkReply::NetworkError type, const QString& error) {

    qDebug() << __FUNCTION__ << type << ":" << error;

}


void OptionsDialog::on_cmbFormatDb_currentTextChanged(const QString &arg1)
{
    //qDebug() << __FUNCTION__ << arg1;

    bool enable = arg1 == "SQLSERVER";
    ui->lineSqlHost->setEnabled(enable);
    ui->lineSqlUser->setEnabled(enable);
    ui->lineSqlPassword->setEnabled(enable);
    ui->toolButton->setEnabled(enable);

   // qDebug() << settings[bConfFieldsWrapper::SQLFormat].toString();
}


void OptionsDialog::on_lineSqlHost_editingFinished()
{
    settings[bConfFieldsWrapper::SQLHost] = ui->lineSqlHost->text();
}


void OptionsDialog::on_lineSqlUser_editingFinished()
{
    settings[bConfFieldsWrapper::SQLUser] = ui->lineSqlUser->text();
}


void OptionsDialog::on_toolButton_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString iconPath = checked ? ":/images/img/viewPwd.svg" : ":/images/img/viewPwd1.svg";
    ui->toolButton->setIcon(QIcon(iconPath));
    ui->lineSqlPassword->setEchoMode(echoMode);
}


void OptionsDialog::on_lineHsHost_textChanged(const QString &arg1)
{
    settings[bConfFieldsWrapper::HSHost] = ui->lineHsHost->text();
}


void OptionsDialog::on_lineHsUser_textChanged(const QString &arg1)
{
    settings[bConfFieldsWrapper::HSUser] = ui->lineHsUser->text();
}


void OptionsDialog::on_btnHsPassView_toggled(bool checked)
{
    auto echoMode = checked ? QLineEdit::Normal : QLineEdit::Password;
    QString iconPath = checked ? ":/images/img/viewPwd.svg" : ":/images/img/viewPwd1.svg";
    ui->btnHsPassView->setIcon(QIcon(iconPath));
    ui->lineHsPassword->setEchoMode(echoMode);
}

