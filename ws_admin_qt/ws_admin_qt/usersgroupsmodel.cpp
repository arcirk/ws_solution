#include "usersgroupsmodel.h"

UsersGroupsModel::UsersGroupsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex UsersGroupsModel::index(int row, int column, const QModelIndex &parent) const
{
    return this->index(row, column, parent);
}

QModelIndex UsersGroupsModel::parent(const QModelIndex &index) const
{
    return this->parent(index);
}

int UsersGroupsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return this->rowCount(parent);
}

int UsersGroupsModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return this->columnCount(parent);
}

QVariant UsersGroupsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
