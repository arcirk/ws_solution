#ifndef DIALOGCONNECTION_H
#define DIALOGCONNECTION_H

#include <QDialog>
#include "settings.h"
#include <QMap>
#include <qmlwebsocket.h>

namespace Ui {
class DialogConnection;
}

class DialogConnection : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConnection(Settings* sett, QMap<QString, QVariant>& clientSett, QWidget *parent = nullptr);
    ~DialogConnection();

    void accept() override;
    QString pwd();

private:
    Ui::DialogConnection *ui;
    Settings* _sett;
    QString _pwd;
    QMap<QString, QVariant>& _clientSett;
    void formControl();

signals:

    void reconnect(Settings* sett, const QString& pwd);
private slots:
    void on_lineServer_textChanged(const QString &arg1);
    void on_btnViewPwd_toggled(bool checked);
    void on_linePwd_textChanged(const QString &arg1);
    void on_btnWsPwd_toggled(bool checked);
    void on_checkMode_toggled(bool checked);
    void on_pwdEdit_toggled(bool checked);
    void on_chkUseSettingsFromHttp_toggled(bool checked);
    void on_btnHttpPwd_toggled(bool checked);
    void on_btnCustomWsUserEdit_clicked();
    void on_chkUseSettingsFromHttp_clicked();
    void on_btnCustomWsUserEdit_toggled(bool checked);
};

#endif // DIALOGCONNECTION_H
