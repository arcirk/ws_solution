#include "userdialog.h"
#include "ui_userdialog.h"

#include "iws_client.h"
#include "directoryinfobasedialog.h"

UserDialog::UserDialog(QWidget *parent, Ui::user_info * usr_info) :
    QDialog(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);

    new_user = usr_info->new_user;

    _usr_info = usr_info;

    if(new_user){
        _usr_info->uuid = QString::fromStdString(random_uuid());
    }

    ui->cmbRole->addItem("admin", "admin");
    ui->cmbRole->addItem("user", "user");

    init_from_user_info();

    if (new_user)
    {
        ui->btnEditPwd->setChecked(true);
        on_btnEditPwd_clicked(true);
    }
}

UserDialog::~UserDialog()
{
    delete ui;
}

void UserDialog::on_toolButton_clicked()
{
    auto * dlg = new DirectoryInfoBaseDialog(this);
    dlg->setModal(true);
    dlg->show();
}

void UserDialog::init_from_user_info() {
    ui->txtPresentation->setText(_usr_info->pres);
    ui->cmbRole->setCurrentText(_usr_info->role);
    ui->txtUUID->setText(_usr_info->uuid);
    ui->txtUserName->setText(_usr_info->name);
    ui->cmbRole->setCurrentText(_usr_info->role);
    this->setWindowTitle(_usr_info->name);
}
void UserDialog::on_btnEditPwd_clicked(bool checked)
{
    ui->txtPassword->setEnabled(checked);
}


void UserDialog::on_buttonBox_accepted()
{
    _usr_info->name = ui->txtUserName->text();
    _usr_info->pres = ui->txtPresentation->text();
    _usr_info->uuid = ui->txtUUID->text();
    _usr_info->role = ui->cmbRole->currentText();
    _usr_info->accepted = true;

    if(ui->btnEditPwd->isChecked())
        _usr_info->password = ui->txtPassword->text();

}

