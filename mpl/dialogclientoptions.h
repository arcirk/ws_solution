#ifndef DIALOGCLIENTOPTIONS_H
#define DIALOGCLIENTOPTIONS_H

#include <QDialog>
#include "profilemanager.h"
#include <user.h>
#include <QJsonDocument>
#include <QJsonObject>
#include "mplsettings.h"
#include "profilemanager.h"

namespace Ui {
class DialogOptions;
}

class DialogClientOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogClientOptions(CertUser* usr, ProfileManager * mpl, QWidget *parent = nullptr);
    ~DialogClientOptions();

    QJsonObject getOptionsCache();

private slots:
    void onChBindCertificateToggled(int state);
    void on_btnHttpPwd_toggled(bool checked);

    void on_btnWsPwd_toggled(bool checked);

    void on_btnCustomWsUserEdit_clicked();

    void on_btnViewPwd_toggled(bool checked);

    void on_checkMode_toggled(bool checked);

    void on_pwdEdit_clicked();

    void on_pwdEdit_toggled(bool checked);

    void on_btnCustomWsUserEdit_toggled(bool checked);

    void on_linePwd_textChanged(const QString &arg1);


private:
    Ui::DialogOptions *ui;
    QJsonObject _obj;
    CertUser * _usr;
    ProfileManager * _mpl;
    QString ch;
    QString _hash;
    int method;
    void setFromUserSettings();
    void formControl();
};

#endif // DIALOGCLIENTOPTIONS_H
