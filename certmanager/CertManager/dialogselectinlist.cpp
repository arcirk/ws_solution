#include "dialogselectinlist.h"
#include "ui_dialogselectinlist.h"
#include <QStringListModel>
#include <QMessageBox>

DialogSelectInList::DialogSelectInList(const QStringList& list, const QString& title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectInList)
{
    ui->setupUi(this);
    auto  m_list = new QStringListModel(list);
    ui->tableView->setModel(m_list);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle(title);
}

DialogSelectInList::DialogSelectInList(QAbstractTableModel *list, const QString &title, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogSelectInList)

{
    ui->setupUi(this);

    ui->tableView->setModel(list);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle(title);
}

DialogSelectInList::DialogSelectInList(QAbstractItemModel *list, const QString &title, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogSelectInList)
{
    ui->setupUi(this);

    ui->tableView->setModel(list);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle(title);
}

DialogSelectInList::~DialogSelectInList()
{
    delete ui;
}

void DialogSelectInList::accept()
{

    auto index = ui->tableView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
        return;
    }

    _result = index.data().toString();

    QDialog::accept();
}

QString DialogSelectInList::dialogResult()
{
    return _result;
}
