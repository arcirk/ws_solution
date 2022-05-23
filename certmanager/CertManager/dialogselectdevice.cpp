#include "dialogselectdevice.h"
#include "ui_dialogselectdevice.h"


DialogSelectDevice::DialogSelectDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectDevice)
{
    ui->setupUi(this);
}

DialogSelectDevice::~DialogSelectDevice()
{
    delete ui;
}

void DialogSelectDevice::on_DialogSelectDevice_accepted()
{

}

