#include "userdialog.h"
#include "ui_userdialog.h"

#include "iws_client.h"
#include "directoryinfobasedialog.h"

UserDialog::UserDialog(QWidget *parent, Ui::user_info * usr_info) :
    QDialog(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);

    new_user = false;

    if(!usr_info){
        _usr_info = new Ui::user_info();
        _usr_info->uuid = QString::fromStdString(arc_json::random_uuid());
        new_user = true;
        this->setWindowTitle("Новый пользователь");
    }

    init_from_user_info();
}

UserDialog::~UserDialog()
{
    if (new_user)
        delete _usr_info;

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
   //ui->txtRole->setText(_usr_info->);
    ui->txtUUID->setText(_usr_info->uuid);
    ui->txtUserName->setText(_usr_info->name);
    this->setWindowTitle(_usr_info->name);
}