#include "dialogcontainername.h"
#include "ui_dialogcontainername.h"

DialogContainerName::DialogContainerName(const QString& currName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogContainerName)
{
    ui->setupUi(this);

    int ind = currName.indexOf("@");

    ui->lineKeyName->setText(currName.left(ind + 1));
    ui->lineContainerName->setText(currName.right(currName.length() - (ind+1)));

    setWindowTitle("Имя контейнера");
}

DialogContainerName::~DialogContainerName()
{
    delete ui;
}

void DialogContainerName::on_lineContainerName_textChanged(const QString &arg1)
{

}

