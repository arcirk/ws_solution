#include "chatboxdialog.h"
#include "ui_chatboxdialog.h"
#include <QQmlComponent>

ChatBoxDialog::ChatBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatBoxDialog)
{
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/Chat.qml")));
}

ChatBoxDialog::~ChatBoxDialog()
{
    delete ui;
}
