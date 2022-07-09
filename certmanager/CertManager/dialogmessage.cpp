#include "dialogmessage.h"
#include "ui_dialogmessage.h"

DialogMessage::DialogMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMessage)
{
    ui->setupUi(this);
}

DialogMessage::~DialogMessage()
{
    delete ui;
}

QString DialogMessage::resultMessage() const
{
    return _message;

}

void DialogMessage::accept()
{
    _message = ui->textEdit->toPlainText();
    QDialog::accept();
}
