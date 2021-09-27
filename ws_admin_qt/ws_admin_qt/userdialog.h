#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <utility>
#include "iws_client.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {

struct user_info{
public:
    QString name;
    QString pres;
    QString hash;
    QString password;
    QString uuid;
    QString role;
    QString parent;
    bool new_user;
    bool accepted;

    ~user_info()= default;
    explicit user_info(QString _name = "Новый пользователь",
              QString _uuid = QString::fromStdString(arc_json::nil_uuid()),
              QString _pres = "Новый пользователь",
              QString _role = "user",
              QString _parent = "")
        : name(std::move(_name)),
          pres(std::move(_pres)),
          uuid(std::move(_uuid)),
          role(std::move(_role)),
          parent(std::move(_parent))
    {
        password = "";
        hash = "";
        new_user = false;
        accepted = false;

    }

    std::string to_json(){

        QJsonObject jsonObject = QJsonObject();
        jsonObject.insert("user", name);
        jsonObject.insert("pwd", password);
        jsonObject.insert("role", role);
        jsonObject.insert("uuid", uuid);
        jsonObject.insert("perf", pres);
        jsonObject.insert("parent", parent);

        return QJsonDocument(jsonObject).toJson(QJsonDocument::Indented).toStdString();
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

    void on_btnEditPwd_clicked(bool checked);

    void on_buttonBox_accepted();

private:
    Ui::UserDialog *ui;
    Ui::user_info * _usr_info;
    bool new_user;

    void init_from_user_info();
};

#endif // USERDIALOG_H
