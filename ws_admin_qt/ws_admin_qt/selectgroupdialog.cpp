#include "selectgroupdialog.h"
#include "ui_selectgroupdialog.h"

SelectGroupDialog::SelectGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectGroupDialog)
{
    ui->setupUi(this);
}

SelectGroupDialog::~SelectGroupDialog()
{
    delete ui;
}
