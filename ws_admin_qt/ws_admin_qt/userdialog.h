#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <utility>
#include "iws_client.h"

namespace Ui {

struct user_info{
public:
    QString name;
    QString pres;
    QString hash;
    QString password;
    QString uuid;
    QString role;

    ~user_info()= default;
    explicit user_info(QString _name = "NewUser",
              QString _uuid = QString::fromStdString(arc_json::nil_uuid()),
              QString _pres = "NewUser",
              QString _role = "user")
        : name(std::move(_name)),
          pres(std::move(_pres)),
          uuid(std::move(_uuid)),
          role(std::move(_role))
    {
        password = "";
        hash = "";
    }
};

class UserDialog;
}

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr, Ui::user_info * usr_info = nullptr);
    ~UserDialog();

private slots:
    void on_toolButton_clicked();

private:
    Ui::UserDialog *ui;
    Ui::user_info * _usr_info;
    bool new_user;

    void init_from_user_info();
};

#endif // USERDIALOG_H
