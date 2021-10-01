#include "chatboxdialog.h"
#include "ui_chatboxdialog.h"
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>

ChatBoxDialog::ChatBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatBoxDialog)
{
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/Chat.qml")));

    QStringListModel listModel;
    QStringList values;
    values << "Option 1" << "Option 2" << "Option 3";

    listModel.setStringList(values);

    ui->quickWidget->engine()->rootContext()->setContextProperty("listModel", &listModel);
}

ChatBoxDialog::~ChatBoxDialog()
{
    delete ui;
}
