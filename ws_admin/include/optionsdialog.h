#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <clientsettings.h>
#include <qwebdav.h>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent);
    ~OptionsDialog() override;

private slots:
    void on_chkSrvLocalInstall_toggled(bool checked);

    void on_chkRunAsService_toggled(bool checked);

    void on_txtWorkingDirectory_textChanged();

    void on_OptionsDialog_accepted();

    void on_btnSelectFolder_clicked();

    void on_ServerName_editingFinished();

    void on_ServerHost_editingFinished();

    void on_ServerPort_valueChanged(double arg1);

    void on_RootUser_editingFinished();

    void on_btnViewPwd_toggled(bool checked);

    void on_editPwd_toggled(bool checked);

    void on_Password_editingFinished();

    void on_chkSavePwd_toggled(bool checked);

    void on_chkAutoConnect_toggled(bool checked);

    void on_radioUseLocalFolder_toggled(bool checked);

    void on_txtLocalFilesFolder_textChanged();

    void on_lineAddressWebDav_editingFinished();

    void on_chSSL_toggled(bool checked);

    void on_lineEdtWebDAvUser_editingFinished();

    void on_lineEdtWebDavPassword_editingFinished();

    void on_btnViewDavPwd_toggled(bool checked);

    void on_btnVerifyWebDav_clicked();

    void onWebDavError(QNetworkReply::NetworkError type, const QString& error);

    void on_cmbFormatDb_currentTextChanged(const QString &arg1);

    void on_lineSqlHost_editingFinished();

    void on_lineSqlUser_editingFinished();

    void on_toolButton_toggled(bool checked);

private:
    Ui::OptionsDialog *ui;
    ClientSettings settings;;
    QWebdav * qWebdav;
};

#endif // OPTIONSDIALOG_H
