#include "../include/selectgroupdialog.h"

#ifndef USE_QT_CREATOR
#include "../form/ui_selectgroupdialog.h"
#else
#include "ui_selectgroupdialog.h"
#endif
#include <QTreeWidget>
#include <QStandardItemModel>
#include <QMessageBox>

SelectGroupDialog::SelectGroupDialog(QWidget *parent, QAbstractItemModel* model) :
    QDialog(parent),
    ui(new Ui::SelectGroupDialog)
{
    ui->setupUi(this);

    ui->treeGroup->setModel(model);
    ui->treeGroup->expandAll();
    dialogue_management();

    dlgAccepted = false;
}

SelectGroupDialog::~SelectGroupDialog()
{
    delete ui;
}

void SelectGroupDialog::on_treeGroup_activated(const QModelIndex &index)
{
    qDebug() << "on_treeGroup_activated";
}


void SelectGroupDialog::on_SelectGroupDialog_accepted()
{
    if (!ui->treeGroup->currentIndex().isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбрана группа!");
    }

    selected_group_uuid = "";

    QModelIndexList indexes = ui->treeGroup->selectionModel()->selectedIndexes();
    if (indexes.size() > 0) {
        QModelIndex selectedIndex = indexes.at(index_uuid);
        auto data = ui->treeGroup->model()->data(selectedIndex);
        selected_group_uuid = data.toString();
        //qDebug() << selected_group_uuid;
    }
    dlgAccepted = true;
}

void SelectGroupDialog::dialogue_management() {

    ui->treeGroup->model()->setHeaderData(0, Qt::Horizontal,"Группы");

    for (int i = 1; i < ui->treeGroup->model()->columnCount(); ++i) {
        QVariant data = ui->treeGroup->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole);
        if (data.toString() == "Ref")
            index_uuid = i;
        ui->treeGroup->setColumnHidden(i, true);
    }

}
