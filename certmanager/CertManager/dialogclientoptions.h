#ifndef DIALOGCLIENTOPTIONS_H
#define DIALOGCLIENTOPTIONS_H

#include <QDialog>
#include <user.h>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class DialogOptions;
}

class DialogClientOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogClientOptions(CertUser* usr, QWidget *parent = nullptr);
    ~DialogClientOptions();

private slots:
    void onChBindCertificateToggled(int state);

private:
    Ui::DialogOptions *ui;
    QJsonObject _obj;
    CertUser * _usr;
    void setFromUserSettings();
    void formControl();
};

#endif // DIALOGCLIENTOPTIONS_H
