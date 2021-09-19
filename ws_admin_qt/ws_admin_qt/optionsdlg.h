#ifndef OPTIONSDLG_H
#define OPTIONSDLG_H

#include <QDialog>
#include "AppSettings.h"

namespace Ui {
class OptionsDlg;
}

class OptionsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDlg(appSettings * settings, QWidget *parent = nullptr);
    ~OptionsDlg();



private slots:
    void on_btnSelectDir_clicked();

    void on_buttonBox_accepted();

    void on_editPwd_clicked(bool checked);

private:
    Ui::OptionsDlg *ui;
    appSettings * _settings;
    QString Hash;
};

#endif // OPTIONSDLG_H
