#include "groupsmodel.h"

GroupsModel::GroupsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int GroupsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_data.size();
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case UUIDRole:
        return QVariant(m_data[index.row()]->uuid);
    case TextRole:
        return QVariant(m_data[index.row()]->name);
    case ParentRole:
        return QVariant(m_data[index.row()]->parent);
    default:
        return QVariant();
    }
 }


QHash<int, QByteArray> GroupsModel::roleNames() const
{

    QHash<int, QByteArray> names;
    names[UUIDRole] = "uuid";
    names[TextRole] = "text";
    names[ParentRole] = "parent";

    return names;
}

void GroupsModel::setRows()
{

    //ToDo: получить данные с сервера

    m_data.clear();

//    QStringList list;
//    list << "Ангарск" << "Иркутск" << "Улан-Удэ" << "Чита" << "Хабаровск" << "Владивосток";
//    for (int i = 0; i < list.length() ; ++i) {
//        m_data.append(new groupe_info(QString::number(i), QString(list[i])));
//    }

    m_data.append(new groupe_info(QString::number(1), "Ангарск", "0"));
    m_data.append(new groupe_info(QString::number(2), "Отдел продаж", "1"));
    m_data.append(new groupe_info(QString::number(3), "Склад", "1"));
    m_data.append(new groupe_info(QString::number(4), "Иркутск", "0"));
    m_data.append(new groupe_info(QString::number(5), "Отдел продаж", "4"));
    m_data.append(new groupe_info(QString::number(6), "Склад", "4"));

}

