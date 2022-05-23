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
    ui->listView->setModel(m_list);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle(title);
}

DialogSelectInList::~DialogSelectInList()
{
    delete ui;
}

void DialogSelectInList::accept()
{

    auto index = ui->listView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран пользователь!");
        return;
    }

    _result = index.data().toString();
}

QString DialogSelectInList::dialogResult()
{
    return _result;
}
