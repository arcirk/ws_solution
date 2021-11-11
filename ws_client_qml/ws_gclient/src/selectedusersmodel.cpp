#include "../include/selectedusersmodel.h"
#include <QJsonObject>
#include <QString>

SelectedUsersModel::SelectedUsersModel(const SelectedUsersModel::Header& header, QObject * parent )
    : QAbstractTableModel( parent )
    , m_header( header )
{

}

bool SelectedUsersModel::setJson(const QJsonDocument &json)
{
    return setJson( json.array() );
}

bool SelectedUsersModel::setJson( const QJsonArray& array )
{
    beginResetModel();
    m_json = array;
    endResetModel();
    return true;
}

QVariant SelectedUsersModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int SelectedUsersModel::rowCount(const QModelIndex &parent ) const
{
    return m_json.size();
}

int SelectedUsersModel::columnCount(const QModelIndex &parent ) const
{
    return m_header.size();
}

QJsonObject SelectedUsersModel::getJsonObject( const QModelIndex &index ) const
{
    const QJsonValue& value = m_json[index.row() ];
    return value.toObject();
}

QVariant SelectedUsersModel::data( const QModelIndex &index, int role ) const
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

QHash<int, QByteArray> SelectedUsersModel::roleNames() const
{

    QHash<int, QByteArray> names;

    for (int i = 0; i < m_header.size(); ++i) {
        const QString& key = m_header[i]["index"];
        names[Qt::UserRole + i] = key.toUtf8();
    }

    return names;
}

int SelectedUsersModel::addRow(const QString &uuid, const QString &name)
{

    if(uuid.isEmpty())
        return 0;

    for (auto itr = m_json.begin(); itr < m_json.end() ; ++itr) {
        auto item = itr->toObject();
        if(item.find("uuid").value().toString() == uuid)
            return iCount;
    }

    QJsonObject msg = QJsonObject();
    msg.insert("uuid", uuid);
    msg.insert("name", name);
    iCount++;
    msg.insert("indexDoc", iCount);

    beginResetModel();
    m_json.push_front(msg);
    endResetModel();

    return iCount;

}

void SelectedUsersModel::setCurrentIndex(int index)
{
    beginResetModel();
    m_currentIndex = index;
    endResetModel();
}

int SelectedUsersModel::currentIndex()
{
    return m_currentIndex;
}

void SelectedUsersModel::remove(int index)
{
    if (m_currentIndex ==  index) {
        m_currentIndex = 0;
    }
    for(int i = 0; i < m_json.size(); ++i ){
        QJsonObject obj = m_json.at(i).toObject();
        if (obj["indexDoc"].toInt() == index) {
            beginResetModel();
            m_json.removeAt(i);
            endResetModel();
            return;
        }
    }
}


//int SelectedUsersModel::getCurrentIndex()
//{
//    return this->index();
//}

//MessageListModel *SelectedUsersModel::getItemListModel(const QString &user_uuid)
//{
//    auto item = m_listModels.find(user_uuid);
//    if (item != m_listModels.end()) {
//        auto model = item.value();
//        if (model) {
//            qDebug() << model->rowCount();
//            //return model;

//        }
//    }

//    return nullptr;
//}
