#include "listmessagesmodel.h"
#include <QFileInfo>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>

ListMessagesModel::ListMessagesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    loadListMessages();
}

QVariant ListMessagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

int ListMessagesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return jsonDocument.array().count();
}

int ListMessagesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ListMessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

void ListMessagesModel::loadListMessages()
{

    //ToDo: временно
    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "messages.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
      return;
    }

    QByteArray saveData = jsonFile.readAll();

    jsonDocument = QJsonDocument::fromJson(saveData);

}

QString ListMessagesModel::recipient() const
{
    return m_recipient;
}

void ListMessagesModel::setRecipient(const QString &recipient)
{
    if (recipient == m_recipient)
        return;

//    m_recipient = recipient;

//    const QString filterString = QString::fromLatin1(
//        "(recipient = '%1' AND author = 'Me') OR (recipient = 'Me' AND author='%1')").arg(m_recipient);
//    setFilter(filterString);
//    select();

    //    emit recipientChanged();
}

QHash<int, QByteArray> ListMessagesModel::roleNames() const
{
    QHash<int, QByteArray> names;
//    names[Qt::UserRole] = "FirstField";
//    names[Qt::UserRole + 1] = "Ref";
//    names[Qt::UserRole + 2] = "SecondField";
//    names[Qt::UserRole + 3] = "_id";
//    names[Qt::UserRole + 4] = "date";
//    names[Qt::UserRole + 5] = "message";
//    names[Qt::UserRole + 6] = "token";
    for (int i; i < msgFields.size(); ++i) {
        QString field = msgFields[(StructMeassage)i];
        names[Qt::UserRole + 1] = field.toUtf8();
    }
    return names;

}

void ListMessagesModel::sendMessage(const QString &recipient, const QString &message)
{
//    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

//    QSqlRecord newRecord = record();
//    newRecord.setValue("author", "Me");
//    newRecord.setValue("recipient", recipient);
//    newRecord.setValue("timestamp", timestamp);
//    newRecord.setValue("message", message);
//    if (!insertRecord(rowCount(), newRecord)) {
//        qWarning() << "Failed to send message:" << lastError().text();
//        return;
//    }

    //submitAll();
}
