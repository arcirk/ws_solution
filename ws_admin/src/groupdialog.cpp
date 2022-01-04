#include "../include/groupdialog.h"
#include "../form/ui_groupdialog.h"
#include <qmlwebsocket.h>

GroupDialog::GroupDialog(QWidget *parent, Ui::group_info* info) :
    QDialog(parent),
    ui(new Ui::GroupDialog)
{
    ui->setupUi(this);

    if (info->new_group){
        if (info->parent_uuid.isEmpty()){
            info->parent_uuid = bWebSocket::nilUuid();
            info->parent_name = "root";
        }
        info->group_name = "Новая группа";
        info->group_presentation = "Новая группа";
        info->group_uuid = bWebSocket::randomUuid();

    }

    ui->txtNameGroup->setText(info->group_name);
    ui->txtParent->setText(info->parent_name);
    ui->txtPresentationGroup->setText(info->group_presentation);

    _info = info;
}

GroupDialog::~GroupDialog()
{
    delete ui;
}

void GroupDialog::on_GroupDialog_accepted()
{
    _info->accepted = true;
    _info->group_name = ui->txtNameGroup->text();
    _info->group_presentation = ui->txtPresentationGroup->text();
}

