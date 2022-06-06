#include "dialogcontainername.h"
#include "ui_dialogcontainername.h"

DialogContainerName::DialogContainerName(const QString& currName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogContainerName)
{
    ui->setupUi(this);

    int ind = currName.indexOf("@");

    key_name = currName.left(ind + 1);
    ui->lineKeyName->setText(key_name);
    ui->lineContainerName->setText(currName.right(currName.length() - (ind+1)));

    setWindowTitle("Имя контейнера");
}

DialogContainerName::~DialogContainerName()
{
    delete ui;
}

void DialogContainerName::accept()
{
    new_name = ui->lineContainerName->text();
    QDialog::accept();
}

QString DialogContainerName::name() const
{
    return new_name;
}

QString DialogContainerName::keyName()
{
    return key_name;
}

void DialogContainerName::on_lineContainerName_textChanged(const QString &arg1)
{

}

