#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "appsettings.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr, appSettings * sett = nullptr);
    ~SettingsDialog();

    bool isAccepted();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingsDialog *ui;
    bool accepted;
    appSettings * _settings;
};

#endif // SETTINGSDIALOG_H
