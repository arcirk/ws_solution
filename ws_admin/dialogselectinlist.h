#ifndef DIALOGSELECTINLIST_H
#define DIALOGSELECTINLIST_H

#include <QDialog>
#include <QAbstractItemModel>
#include <QAbstractTableModel>

namespace Ui {
class DialogSelectinList;
}

class DialogSelectinList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectinList(QAbstractItemModel * model, QWidget *parent = nullptr);
    explicit DialogSelectinList(QAbstractTableModel * model, QWidget *parent = nullptr);
    ~DialogSelectinList();

    void accept() override;

    QStringList dialogResult()
    {
        return _result;
    };


private:
    Ui::DialogSelectinList *ui;
     QStringList _result;
};

#endif // DIALOGSELECTINLIST_H
