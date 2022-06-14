#include "dialogselectinlist.h"
#include "ui_dialogselectinlist.h"
#include <QMessageBox>

DialogSelectinList::DialogSelectinList(QAbstractItemModel * model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectinList)
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle("Форма выбора");
    ui->tableView->resizeColumnsToContents();
}

DialogSelectinList::DialogSelectinList(QAbstractTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectinList)
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle("Форма выбора");
    ui->tableView->resizeColumnsToContents();
}

DialogSelectinList::~DialogSelectinList()
{
    delete ui;
}

void DialogSelectinList::accept()
{
    auto index = ui->tableView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }

    auto model = ui->tableView->model();

    for (int i = 0; i < model->columnCount(); ++i) {
        _result.append(model->index(index.row(), i).data().toString());
    }

    QDialog::accept();
}
