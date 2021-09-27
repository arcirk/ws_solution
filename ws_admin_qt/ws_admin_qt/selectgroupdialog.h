#ifndef SELECTGROUPDIALOG_H
#define SELECTGROUPDIALOG_H

#include <QDialog>

namespace Ui {
class SelectGroupDialog;
}

class SelectGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGroupDialog(QWidget *parent = nullptr);
    ~SelectGroupDialog();

private:
    Ui::SelectGroupDialog *ui;
};

#endif // SELECTGROUPDIALOG_H
