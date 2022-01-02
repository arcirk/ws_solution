#include "../include/directoryinfobasedialog.h"
#include "../form/ui_directoryinfobasedialog.h"

DirectoryInfoBaseDialog::DirectoryInfoBaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirectoryInfoBaseDialog)
{
    ui->setupUi(this);
}

DirectoryInfoBaseDialog::~DirectoryInfoBaseDialog()
{
    delete ui;
}
