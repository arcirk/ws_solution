#include "directoryinfobasedialog.h"
#include "ui_directoryinfobasedialog.h"

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
