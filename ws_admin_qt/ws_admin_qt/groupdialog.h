#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>

namespace Ui {
class GroupDialog;
}

class GroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupDialog(QWidget *parent = nullptr);
    ~GroupDialog();

private:
    Ui::GroupDialog *ui;
};

#endif // GROUPDIALOG_H
