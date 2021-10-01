#include "usermessages.h"
#include <QDate>

UserMessagesModel::UserMessagesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int UserMessagesModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    //ToDo: Исправить
    return 100;
}

bool UserMessagesModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool UserMessagesModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void UserMessagesModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant UserMessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case DoneRole:
        return QVariant(false);
    case DateRole:
        return QVariant(QDate::currentDate());
    case Sender:
        return QVariant(QString("Sender"));
    case Recipient:
        return QVariant(QString("Recipient"));
    case Message:
        return QVariant(QString("Message"));
    case Token:
        return QVariant(QString("Token"));
    }

    // FIXME: Implement me!
    return QVariant();
}

bool UserMessagesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags UserMessagesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool UserMessagesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool UserMessagesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

QHash<int, QByteArray> UserMessagesModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[DateRole] = "date";
    names[Sender] = "sender";
    names[Recipient] = "recipient";
    names[Message] = "message";
    names[Token] = "token";

    return names;
}
