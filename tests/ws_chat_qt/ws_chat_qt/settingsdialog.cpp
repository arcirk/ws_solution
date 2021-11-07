#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, appSettings * sett) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->txtHost->setText(sett->ServerHost);
    ui->intPort->setValue(sett->ServerPort);
    _settings = sett;
    accepted = false;

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::isAccepted()
{
    return accepted;
}

void SettingsDialog::on_buttonBox_accepted()
{
    _settings->ServerHost = ui->txtHost->text();
    _settings->ServerPort = ui->intPort->value();
    accepted = true;
}

