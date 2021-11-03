//
// Created by arcady on 03.11.2021.
//

#include "grouptreemodel.h"
#include <QJsonObject>
#include <QString>

GroupTreeModel::GroupTreeModel(const GroupTreeModel::Header& header, QObject * parent )
        : QAbstractTableModel( parent )
        , m_header( header )
{

}

bool GroupTreeModel::setJson(const QJsonDocument &json)
{
    return setJson( json.array() );
}

bool GroupTreeModel::setJson( const QJsonArray& array )
{
    beginResetModel();
    m_json = array;
    endResetModel();
    return true;
}

QVariant GroupTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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