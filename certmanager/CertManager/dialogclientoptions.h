#ifndef DIALOGCLIENTOPTIONS_H
#define DIALOGCLIENTOPTIONS_H

#include <QDialog>
#include <user.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <qjsontablemodel.h>
#include <qproxymodel.h>

namespace Ui {
class DialogOptions;
}

class DialogClientOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogClientOptions(CertUser* usr, QProxyModel* a_certs, QWidget *parent = nullptr);
    ~DialogClientOptions();

    void accept() override;

    QJsonObject getOptionsCache();

private slots:
    void onChBindCertificateToggled(int state);

    void on_btnHttpPwd_toggled(bool checked);

    void on_btnWsPwd_toggled(bool checked);

    void on_btnCustomWsUserEdit_toggled(bool checked);

    void on_btnCustomWsUserEdit_clicked();

    void on_btnViewPwd_toggled(bool checked);

    void on_checkMode_toggled(bool checked);

    void on_pwdEdit_clicked();

    void on_pwdEdit_toggled(bool checked);

private:
    Ui::DialogOptions *ui;
    CertUser * _usr;
    QString ch;
    QString _hash;
    int method;
    QJsonTableModel* _profiles;

    void setFromUserSettings();
    void formControl();
    void setProfoleImage(int index, const QString &imagePath);
    void updateTableImages();
};

#endif // DIALOGCLIENTOPTIONS_H
