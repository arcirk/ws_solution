#include "activeroomsmodel.h"
#include <iws_client.h>

ActiveRoomsModel::ActiveRoomsModel( QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant ActiveRoomsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool ActiveRoomsModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int ActiveRoomsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return rooms.size();
}

QVariant ActiveRoomsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case DoneRole:
        return QVariant(false);
    case TokenRole:
        return QVariant(QString("Token"));
    case SenderRole:
        return QVariant(QString("Sender"));
    case RecipientRole:
        return QVariant(QString("Recipient"));
    case MessagesRole:
        return QVariant(QString("Message"));
    }

    return QVariant();
}

bool ActiveRoomsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ActiveRoomsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool ActiveRoomsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool ActiveRoomsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

QHash<int, QByteArray> ActiveRoomsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[TokenRole] = "date";
    names[SenderRole] = "sender";
    names[RecipientRole] = "recipient";
    names[MessagesRole] = "messages";

    return names;
}

void ActiveRoomsModel::init()
{
    rooms.clear();
    QString rootToken = QString::fromStdString(arc_json::nil_uuid());
    rooms.insert(rootToken, new RoomItemData("Общий чат", rootToken));
}
