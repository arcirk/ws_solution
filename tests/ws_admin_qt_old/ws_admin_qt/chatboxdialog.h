#ifndef CHATBOXDIALOG_H
#define CHATBOXDIALOG_H

#include <QDialog>
#include <QQuickWidget>
#include <QStringListModel>

#include "usersgroupsmodel.h"

namespace Ui {
class ChatBoxDialog;
}

class ChatBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatBoxDialog(QWidget *parent = nullptr, UsersGroupsModel *groupModel = nullptr);
    ~ChatBoxDialog() override;

private:
    Ui::ChatBoxDialog *ui;
    QQuickWidget* quickWidget;
    UsersGroupsModel * _groupModel;
};

#endif // CHATBOXDIALOG_H
