#ifndef DIALOGCONNECTION_H
#define DIALOGCONNECTION_H

#include <QDialog>
#include "settings.h"

namespace Ui {
class DialogConnection;
}

class DialogConnection : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConnection(settings* sett, QWidget *parent = nullptr);
    ~DialogConnection();

    void accept() override;
    QString pwd();

private:
    Ui::DialogConnection *ui;
    settings* _sett;
    QString _pwd;

signals:

    void reconnect(settings* sett, const QString& pwd);
private slots:
    void on_lineServer_textChanged(const QString &arg1);
    void on_btnViewPwd_toggled(bool checked);
    void on_linePwd_textChanged(const QString &arg1);
    void on_checkMode_stateChanged(int arg1);
};

#endif // DIALOGCONNECTION_H
