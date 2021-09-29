#ifndef SELECTGROUPDIALOG_H
#define SELECTGROUPDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>

namespace Ui {
class SelectGroupDialog;
}

class SelectGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGroupDialog(QWidget *parent = nullptr, QAbstractItemModel* model = nullptr);
    ~SelectGroupDialog();

    QString selected_group_uuid;
    bool dlgAccepted;

private slots:
    void on_buttonBox_accepted();

    void on_treeGroup_activated(const QModelIndex &index);

private:
    Ui::SelectGroupDialog *ui;
    int index_uuid = 0;
    void dialogue_management();
};

#endif // SELECTGROUPDIALOG_H
