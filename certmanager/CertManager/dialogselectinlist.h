#ifndef DIALOGSELECTINLIST_H
#define DIALOGSELECTINLIST_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractTableModel>
#include <QAbstractItemModel>

namespace Ui {
class DialogSelectInList;
}

class DialogSelectInList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInList(const QStringList& list, const QString& title, QWidget *parent = nullptr);
    explicit DialogSelectInList(QAbstractTableModel* list, const QString& title, QWidget *parent = nullptr);
    explicit DialogSelectInList(QAbstractItemModel* list, const QString& title, QWidget *parent = nullptr);
    ~DialogSelectInList();

    void accept() override;

    QString dialogResult();

private:
    Ui::DialogSelectInList *ui;
    QString _result;
};

#endif // DIALOGSELECTINLIST_H
