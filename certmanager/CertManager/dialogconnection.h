#ifndef DIALOGCONNECTION_H
#define DIALOGCONNECTION_H

#include <QDialog>
#include "settings.h"
#include <QMap>

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
    QMap<QString, QVariant> _clientSett;
    void formControl();

signals:

    void reconnect(Settings* sett, const QString& pwd);
private slots:
    void on_lineServer_textChanged(const QString &arg1);
    void on_btnViewPwd_toggled(bool checked);
    void on_linePwd_textChanged(const QString &arg1);
    void on_checkMode_stateChanged(int arg1);
    void on_btnWsPwd_toggled(bool checked);
    void on_checkMode_toggled(bool checked);
};

#endif // DIALOGCONNECTION_H
