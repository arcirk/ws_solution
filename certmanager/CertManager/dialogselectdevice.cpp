#include "dialogselectdevice.h"
#include "ui_dialogselectdevice.h"


DialogSelectDevice::DialogSelectDevice(QWidget *parent, const QString& tTitle) :
    QDialog(parent),
    ui(new Ui::DialogSelectDevice)
{
    ui->setupUi(this);

    ui->radioBtnFolder->setChecked(true);
    _currentSelection = -1;
    ui->radioBtnUserRegysty->setText(tTitle);
}

DialogSelectDevice::~DialogSelectDevice()
{
    delete ui;
}

void DialogSelectDevice::accept()
{
    if(ui->radioBtnFolder->isChecked())
        _currentSelection = 0;
    else if(ui->radioBtnRegysty->isChecked())
        _currentSelection = 1;
    else if(ui->radioBtnUserRegysty->isChecked())
        _currentSelection = 2;

    QDialog::accept();
}

int DialogSelectDevice::currentSelection()
{
    return _currentSelection;
}


