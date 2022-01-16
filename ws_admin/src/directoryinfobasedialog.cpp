#include "../include/directoryinfobasedialog.h"

#ifndef USE_QT_CREATOR
#include "../form/ui_directoryinfobasedialog.h"
#else
#include "ui_directoryinfobasedialog.h"
#endif

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
