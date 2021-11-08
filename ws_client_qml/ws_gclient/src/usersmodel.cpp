#include "../include/usersmodel.h"
#include <QJsonObject>
#include <QString>

UsersModel::UsersModel(const UsersModel::Header& header, QObject * parent )
    : QAbstractTableModel( parent )
    , m_header( header )
{

}

bool UsersModel::setJson(const QJsonDocument &json)
{
    return setJson( json.array() );
}

bool UsersModel::setJson( const QJsonArray& array )
{
    beginResetModel();
    m_json = array;
    endResetModel();
    return true;
}

QVariant UsersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole )
    {
        return QVariant();
    }

    switch( orientation )
    {
    case Qt::Horizontal:
        return m_header[section]["title"];
    case Qt::Vertical:
        //return section + 1;
        return QVariant();
    default:
        return QVariant();
    }

}

int UsersModel::rowCount(const QModelIndex &parent ) const
{
    return m_json.size();
}

int UsersModel::columnCount(const QModelIndex &parent ) const
{
    return m_header.size();
}

QJsonObject UsersModel::getJsonObject( const QModelIndex &index ) const
{
    const QJsonValue& value = m_json[index.row() ];
    return value.toObject();
}

QVariant UsersModel::data( const QModelIndex &index, int role ) const
{

    if (role == Qt::UserRole){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "uuid" ))
        {
            return obj["uuid"].toString();
        }

    }else if (role == Qt::UserRole + 1){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "name" ))
        {
            return obj["name"].toString();
        }

    }else if (role == Qt::UserRole + 2){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "indexDoc" ))
        {
            return obj["indexDoc"].toInt();
        }

    }


    switch( role )
    {
    case Qt::DisplayRole:
    {
        QJsonObject obj = getJsonObject( index );
        const QString& key = m_header[index.column()]["index"];
        if( obj.contains( key ))
        {
            QJsonValue v = obj[ key ];

            if( v.isString() )
            {
                return v.toString();
            }
            else if( v.isDouble() )
            {
                return QString::number( v.toDouble() );
            }
            else
            {
                return QVariant();
            }
        }
        else
        {
            return QVariant();
        }
    }
    case Qt::ToolTipRole:
        return QVariant();
    default:
        return QVariant();
    }

}

QHash<int, QByteArray> UsersModel::roleNames() const
{

    QHash<int, QByteArray> names;

    for (int i = 0; i < m_header.size(); ++i) {
        const QString& key = m_header[i]["index"];
        names[Qt::UserRole + i] = key.toUtf8();
    }

    return names;
}

QSortFilterProxyModel *UsersModel::subdivisions()
{
    return _subdivisions;
}

void UsersModel::setSubdivisions(QSortFilterProxyModel *proxy)
{
    _subdivisions = proxy;
}
