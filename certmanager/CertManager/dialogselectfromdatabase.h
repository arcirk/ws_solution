#ifndef DIALOGSELECTFROMDATABASE_H
#define DIALOGSELECTFROMDATABASE_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class DialogSelectFromDataBase;
}

class DialogSelectFromDataBase : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectFromDataBase(QSqlQueryModel * model, QWidget *parent = nullptr);
    ~DialogSelectFromDataBase();

    void accept() override;

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogSelectFromDataBase *ui;
};

#endif // DIALOGSELECTFROMDATABASE_H
