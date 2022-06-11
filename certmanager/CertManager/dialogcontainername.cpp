#include "dialogcontainername.h"
#include "ui_dialogcontainername.h"
#include "keyscontainer.h"

DialogContainerName::DialogContainerName(const QString& currName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogContainerName)
{
    ui->setupUi(this);

    auto cnt = KeysContainer();
    cnt.fromContainerName(currName);

    //int ind = currName.indexOf("@");

    key_name = cnt.keyName();//currName.left(ind + 1);

    ui->lineKeyName->setText(key_name);
    end_date = cnt.notValidAfter();
    end_date.replace(".", "-");
    ui->lineDate->setText(end_date);
    new_name = cnt.name();
    ui->lineContainerName->setText(new_name); //currName.right(currName.length() - (ind+1))


    setWindowTitle("Имя контейнера");
}

DialogContainerName::~DialogContainerName()
{
    delete ui;
}

void DialogContainerName::accept()
{
    _name = ui->lineKeyName->text() + "@" +  ui->lineDate->text() + "-" + ui->lineContainerName->text();
    QDialog::accept();
}

QString DialogContainerName::name() const
{
    return _name;
}

QString DialogContainerName::keyName()
{
    return key_name;
}

void DialogContainerName::on_lineContainerName_textChanged(const QString &arg1)
{

}


void DialogContainerName::on_DialogContainerName_accepted()
{

}

