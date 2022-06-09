#include "dialogselectprofile.h"
#include "ui_dialogselectprofile.h"
#include <QStringListModel>
#include <QMessageBox>

DialogSelectProfile::DialogSelectProfile(const QStringList& list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectProfile)
{
    ui->setupUi(this);

    auto  m_list = new QStringListModel(list);
    ui->listView->setModel(m_list);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

DialogSelectProfile::~DialogSelectProfile()
{
    delete ui;
}

void DialogSelectProfile::accept()
{
    auto index = ui->listView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран элемент!");
        return;
    }

    m_result = index.data().toString();
    QDialog::accept();
}

QString DialogSelectProfile::getResult()
{
    return m_result;
}

