#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>

namespace Ui {

struct group_info{

public:
    QString parent_uuid;
    QString parent_name;
    QString group_name;
    QString group_uuid;
    QString group_presentation;
    bool accepted;
    bool new_group;
    explicit group_info() {
        new_group = false;
        accepted = false;
    }
    ~group_info() = default;

};

class GroupDialog;

}

class GroupDialog : public QDialog
{
    Q_OBJECT

public:

    explicit GroupDialog(QWidget *parent = nullptr, Ui::group_info* info = nullptr);
    ~GroupDialog() override;




private slots:
    void on_buttonBox_accepted();

private:
    Ui::GroupDialog *ui;
    Ui::group_info* _info;

};

#endif // GROUPDIALOG_H
