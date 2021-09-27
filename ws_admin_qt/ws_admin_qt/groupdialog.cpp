#include "groupdialog.h"
#include "ui_groupdialog.h"

GroupDialog::GroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupDialog)
{
    ui->setupUi(this);
}

GroupDialog::~GroupDialog()
{
    delete ui;
}
