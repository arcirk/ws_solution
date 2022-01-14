#include "../include/optionsdialog.h"
#include "../form/ui_optionsdialog.h"

#include <iws_client.h>
#include <QFileDialog>
#include <QMessageBox>
#include <bwebdav.h>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    settings("conf_admin_console.json", false)
{
    ui->setupUi(this);

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

    settings.password = "***";

    //pWebDav= new bWebDav(this, settings.confFileName());
    qWebdav= new QWebdav(this, settings.confFileName());
    QObject::connect(qWebdav, SIGNAL(errorChanged(QNetworkReply::NetworkError, QString)), this, SLOT(onWebDavError(QNetworkReply::NetworkError, QString)));

    //QObject::connect(pWebDav, SIGNAL(verifyRootDirResult(bool, bool)), this, SLOT(onVerifyRootDirResult(bool, bool )));
    //QObject::connect(pWebDav, SIGNAL(createDir(bool, QString)), this, SLOT(onCreateWbDavDirectory(bool, QString )));

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

//    std::string dPwd = settings[bConfFieldsWrapper::WebDavPwd].toString().toStdString();
//    QString pass;
//    if (!dPwd.empty()){
//        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
//    }

    QString pass;
    std::string dPwd = ui->lineEdtWebDavPassword->text().toStdString();
    if (!dPwd.empty()){
        pass = QString::fromStdString(ClientSettings::crypt(dPwd, "my_key"));
    }
    settings[bConfFieldsWrapper::WebDavPwd] = pass;

    settings.save();
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
    settings[bConfFieldsWrapper::ServerName] = ui->ServerName->text();
}

void OptionsDialog::on_ServerHost_editingFinished()
{
    settings[bConfFieldsWrapper::ServerHost] = ui->ServerHost->text();
}

void OptionsDialog::on_ServerPort_valueChanged(double arg1)
{
    settings[bConfFieldsWrapper::ServerPort] = arg1;
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
    //pWebDav->verify();

    //меняем на текущие
    qWebdav->setUser(ui->lineEdtWebDAvUser->text());
    qWebdav->setPassword(ui->lineEdtWebDavPassword->text());
    qWebdav->setHost(ui->lineAddressWebDav->text());
    qWebdav->setSsl(ui->chSSL->isChecked());

    //синхронные вызовы
    bool isConnections = qWebdav->isConnections();
    bool exists = false;
    if(isConnections)
        exists = qWebdav->rootExists();

    if (!isConnections){
        QMessageBox::critical(this, "Ошибка", "Ошибка соединения!");
    }else{
        if (exists){
            QMessageBox::information(this, tr("Успех"),
                                     tr("Успешное подключение!"));
        }else{
            auto result =  QMessageBox::question(this, "Каталог на сервере", "Доступ есть, но основной каталог программы не найден.\n"
                                                                             "Создать каталог?");
            if(result == QMessageBox::Yes){
                bool mkdir = qWebdav->mkdirSynch("");
                if(mkdir)
                    QMessageBox::information(this, tr("Успех"),
                                             tr("Каталог успешно создан!"));
                else
                    QMessageBox::critical(this, tr("Ошибка"),
                                             tr("Ошибка создания каталога!"));
            }
        }
    }

    //восстановим старые настройки
    qWebdav->resetSettings();
}


void OptionsDialog::onWebDavError(QNetworkReply::NetworkError type, const QString& error) {

    qDebug() << type << ":" << error;

}

