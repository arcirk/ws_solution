#include "dialogserveruser.h"
#include "ui_dialogserveruser.h"

DialogServerUser::DialogServerUser(const QJsonObject& object, const QString& parentName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogServerUser)
{
    ui->setupUi(this);

    _obj = object;

    ui->lblGroup->setText(parentName);
    //ui->lblName->set
}

DialogServerUser::~DialogServerUser()
{
    delete ui;
}
