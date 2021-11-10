#include "../include/usersmodel.h"
#include <QJsonObject>
#include <QString>
#include <QFileInfo>
#include <QDir>

UsersModel::UsersModel(QObject *parent)
        : QAbstractTableModel(parent)
{

//    //ToDo: удалить
//    QJsonObject m_currentJsonObject = QJsonObject();
//
//    QString openFileName = "usersCatalog.json";
//
//    QFileInfo fileInfo(openFileName);
//    QDir::setCurrent(fileInfo.path());
//
//    QFile jsonFile(openFileName);
//    if (!jsonFile.open(QIODevice::ReadOnly))
//    {
//        return;
//    }

//    QByteArray saveData = jsonFile.readAll();
//    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));


    _subdivisions = new UsersProxyModel();
    _subdivisions->setSourceModel(this);
    _subdivisions->setRef("00000000-0000-0000-0000-000000000000");
    _subdivisions->setIsGroup(1);

    QString defJson = "{\n"
                      "    \"columns\": ["
                      "        \"FirstField\","
                      "        \"SecondField\","
                      "        \"Ref\","
                      "        \"Parent\","
                      "        \"IsGroup\""
                      "    ],"
                      "    \"rows\": []"
                      "}";

    setJson(QJsonDocument::fromJson(defJson.toUtf8()));

}


bool UsersModel::setJson(const QJsonDocument &json)
{

    if (json.isNull()) {
        return false;
    }

   // beginResetModel();

    auto _header = json.object().value("columns").toArray();

    m_header.clear();

    int i = 0;
    for (auto itr : _header) {
        QString column = itr.toString();
        if (column == "Parent")
            filterIndex = i;
        m_header.push_back( UsersModel::Heading( { {"title",column},    {"index",column} }) );
        i++;
    }

    auto _rows = json.object().value("rows").toArray();
    setJson( _rows );

    //endResetModel();

//    if(_subdivisions)
//        delete _subdivisions;

    return true;
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

//    if (role == Qt::UserRole){
//        QJsonObject obj = getJsonObject( index );
//        if( obj.contains( "uuid" ))
//        {
//            return obj["uuid"].toString();
//        }
//
//    }else if (role == Qt::UserRole + 1){
//        QJsonObject obj = getJsonObject( index );
//        if( obj.contains( "name" ))
//        {
//            return obj["name"].toString();
//        }
//
//    }else if (role == Qt::UserRole + 2){
//        QJsonObject obj = getJsonObject( index );
//        if( obj.contains( "indexDoc" ))
//        {
//            return obj["indexDoc"].toInt();
//        }
//
//    }

    if (role >= Qt::UserRole){
        QJsonObject obj = getJsonObject( index );
        QString key = QString::fromStdString(roleNames()[role].toStdString());
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
        int index = Qt::UserRole + i;
        names[index] = key.toUtf8();
    }

    return names;
}

UsersProxyModel *UsersModel::subdivisions()
{
//    auto names = roleNames();
//    int indexParent = 0;
//    int indexIsGroup = 0;
//    for (int index : names.keys() ) {
//        if(names[index].toStdString() == "Parent"){
//            indexParent = index - Qt::UserRole;
//            break;
//        }
//    }
//    for (int index : names.keys() ) {
//        if(names[index].toStdString() == "IsGroup"){
//            indexIsGroup = index - Qt::UserRole;
//            break;
//        }
//    }

//    _subdivisions->setFilterFixedString("00000000-0000-0000-0000-000000000000");
//    _subdivisions->setFilterKeyColumn(indexParent);

//    _subdivisions->setRef("00000000-0000-0000-0000-000000000000");
//    _subdivisions->setIsGroup(1);

    //beginResetModel();
    //m_json = array;
    //endResetModel();

    return _subdivisions;
}

//void UsersModel::setSubdivisions(UsersProxyModel *proxy)
//{
//    _subdivisions = proxy;
//    _subdivisions->setSourceModel(this);
//}

int UsersModel::getColumnIndex(const QString &name) {

    int result = 0;
    //int i = 0;

    auto names = roleNames();

    for (int iKey : names.keys() ) {
        if (names[iKey].toStdString() == name.toStdString()){
            result = iKey - Qt::UserRole;
            break;
        }
    }

//    for (auto itr : names) {
//        if (itr.toStdString() == name.toStdString()){
//            result = i;
//            break;
//        }
//        i++;
//    }

    return result;
}

