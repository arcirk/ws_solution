#include "qmlmessage.h"

QmlMessage::QmlMessage(QObject *parent) : QObject(parent)
{

}

void QmlMessage::setCommand(const QString &cmd)
{
    Command = cmd;
}

QString QmlMessage::getCommand() const
{
    return Command;
}

void QmlMessage::setMessage(const QString &msg)
{
    Message = msg;
}

QString QmlMessage::getMessage() const
{
    return Message;
}

void QmlMessage::setUuidSender(const QString &uuid)
{
    uuidSender = uuid;
}

void QmlMessage::setUuidRecipient(const QString &uuid)
{
    uuidRecipient = uuid;
}

void QmlMessage::setDateMessage(int _date)
{
    DateMessage = _date;
}

QString QmlMessage::getUuidSender() const
{
    return uuidSender;
}

QString QmlMessage::getUuidRecipient() const
{
    return uuidRecipient;
}

int QmlMessage::getDateMessage() const
{
    return DateMessage;
}
