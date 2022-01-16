#include "../include/aboutdialog.h"
#ifndef USE_QT_CREATOR
#include "../form/ui_aboutdialog.h"
#else
#include "ui_aboutdialog.h"
#endif
//#include "ui_aboutdialog.h"
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QIcon appIcon(":/img/ws_admin.ico");
    QApplication::setWindowIcon(appIcon);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
