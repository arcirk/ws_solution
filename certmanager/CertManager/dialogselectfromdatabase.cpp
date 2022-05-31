#include "dialogselectfromdatabase.h"
#include "ui_dialogselectfromdatabase.h"
#include <QMessageBox>

DialogSelectFromDataBase::DialogSelectFromDataBase(QSqlQueryModel * model, const QString title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectFromDataBase)
{
    ui->setupUi(this);

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();

    setWindowTitle(title);
}

DialogSelectFromDataBase::~DialogSelectFromDataBase()
{
    delete ui;
}

void DialogSelectFromDataBase::accept()
{
    auto index = ui->tableView->currentIndex();
    if(!index.isValid())
    {
         QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
         return;
    }

    QDialog::accept();
}

void DialogSelectFromDataBase::on_tableView_doubleClicked(const QModelIndex &index)
{
    accept();
}

