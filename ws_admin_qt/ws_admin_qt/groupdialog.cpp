#include "groupdialog.h"
#include "ui_groupdialog.h"
#include "iws_client.h"

GroupDialog::GroupDialog(QWidget *parent, Ui::group_info* info) :
    QDialog(parent),
    ui(new Ui::GroupDialog)
{
    ui->setupUi(this);

    if (info->new_group){
        if (info->parent_uuid.isEmpty()){
            info->parent_uuid = QString::fromStdString(arc_json::nil_uuid());
            info->parent_name = "root";
        }
        info->group_name = "Новая группа";
        info->group_presentation = "Новая группа";
        info->group_uuid = QString::fromStdString(arc_json::random_uuid());

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

void GroupDialog::on_buttonBox_accepted()
{
    _info->accepted = true;    
    _info->group_name = ui->txtNameGroup->text();
    _info->group_presentation = ui->txtPresentationGroup->text();
}

