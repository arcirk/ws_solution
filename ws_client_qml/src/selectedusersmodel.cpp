#include "../include/selectedusersmodel.h"
#include <QJsonObject>
#include <QString>

#pragma warning(disable:4100)

SelectedUsersModel::SelectedUsersModel(QObject * parent )
    : QAbstractTableModel( parent )
{
    m_currentRow = "";
    m_header.push_back( SelectedUsersModel::Heading( { {"title","uuid"},    {"index","uuid"} }) );
    m_header.push_back( SelectedUsersModel::Heading( { {"title","name"},   {"index","name"} }) );
    m_header.push_back( SelectedUsersModel::Heading( { {"title","draft"},   {"index","draft"} }) ); //черновик (набранный но не отправленный текст)
    m_header.push_back( SelectedUsersModel::Heading( { {"title","unreadMessages"},   {"index","unreadMessages"} }) ); //не прочитанные сообщения
    m_header.push_back( SelectedUsersModel::Heading( { {"title","active"},   {"index","active"} }) );
    m_header.push_back( SelectedUsersModel::Heading( { {"title","token"},   {"index","token"} }) );
    getHeaderJsonObject();
    m_json = QJsonArray();
    setJson(QJsonArray());
}

bool SelectedUsersModel::setJson(const QJsonDocument &json)
{
    if (json.isNull()) {
        return false;
    }

    QJsonObject jsonObj = json.object();

    if(jsonObj.isEmpty()){        
        return false;
    }else{
        QJsonObject obj = jsonObj.value("chats").toObject();
        QJsonArray __header = obj.value("columns").toArray();
        QList<QString> list; //Для новых полей
        if(!__header.isEmpty() && __header.count() > 0){
           m_header.clear();
            int i = 0;
            for (auto itr : __header) {
                QString column = itr.toString();
                m_header.push_back( MessageListModel::Heading( { {"title",column},    {"index",column} }) );
                list.push_back(column);
                i++;
            };
            ///
            int ind = list.indexOf("unreadMessages");
            if (ind == -1){
                //list.push_back("unreadMessages");
                m_header.push_back( MessageListModel::Heading( { {"title","unreadMessages"},    {"index","unreadMessages"} }) );
            }
            ind = list.indexOf("active");
            if (ind == -1){
                //list.push_back("active");
                m_header.push_back( MessageListModel::Heading( { {"title","active"},    {"index","active"} }) );
            }
            ind = list.indexOf("token");
            if (ind == -1){
                //list.push_back("active");
                m_header.push_back( MessageListModel::Heading( { {"title","token"},    {"index","token"} }) );
            }
            getHeaderJsonObject();
        }


        m_currentRow = jsonObj.value("currentRow").toString();
        ///
        /// \brief _rows
        ///
        auto _rows = obj.value("rows").toArray();
        setJson( _rows );

   }

    return true;
}

bool SelectedUsersModel::setJson( const QJsonArray& array )
{
    beginResetModel();
    m_json = array;
    if(rowCount() > 0 && m_currentRow == ""){
        m_currentRow = data((index(0,0)), Qt::UserRole).toString();
    }
    endResetModel();

    emit currentRowChanged();

    return true;
}

QVariant SelectedUsersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (m_header.size() == 0)
        return QVariant();

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
    return (int)m_json.size();
}

int SelectedUsersModel::columnCount(const QModelIndex &parent ) const
{
    return (int)m_header.size();
}

QJsonObject SelectedUsersModel::getJsonObject( const QModelIndex &index ) const
{
    const QJsonValue& value = m_json[index.row() ]; 
    return value.toObject();
}

void SelectedUsersModel::getHeaderJsonObject()
{
    _header = QJsonArray();
    for(auto itr : m_header){
        if(itr.size() > 0){
            _header.push_back(itr["index"]);
        }
    }
}

void SelectedUsersModel::resetCountUnReadMsg()
{
    if(m_currentRow.length() > 0){
        int col = getColumnIndex("unreadMessages");
        if(col == 0)
            return;
        QModelIndex ind = item(m_currentRow, getColumnIndex("unreadMessages"));
        if(ind.isValid())
            setRowValue(ind, 0);
    }
}

QVariant SelectedUsersModel::data( const QModelIndex &index, int role ) const
{
    if (m_json.size() == 0)
        QVariant();

    if (role >= Qt::UserRole){
        QJsonObject obj = getJsonObject( index );
        QString key = QString::fromStdString(roleNames()[role].toStdString());
        if (key == "token")
            qDebug() << key;

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
            else if( v.isBool() )
            {
                return v.toBool();
            }else{
                if(key == "unreadMessages")
                    return 0;
                else if (key == "active")
                    return false;
                else
                    return QString();
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

QHash<int, QByteArray> SelectedUsersModel::roleNames() const
{

    QHash<int, QByteArray> names;

    for (int i = 0; i < m_header.size(); ++i) {
        const QString& key = m_header[i]["index"];
        names[Qt::UserRole + i] = key.toUtf8();
    }

    return names;
}

void SelectedUsersModel::addRow(const QString &uuid, const QString &name, bool unActive )
{

    if(uuid.isEmpty())
        return;

    if(uuid == _user_uuid) //основной пользователь
        return;

    //пользователь уже есть в списке
    for (auto itr = m_json.begin(); itr < m_json.end() ; ++itr) {
        auto item = itr->toObject();
        if(item.find("uuid").value().toString() == uuid){
            m_currentRow = uuid;
            emit currentRowChanged();
            return;
        }

    }

    QJsonObject msg = QJsonObject();
    msg.insert("uuid", uuid);
    msg.insert("name", name);
    msg.insert("draft", "");
    msg.insert("unreadMessages", 0); //ToDo: добавить запрос
    msg.insert("active", false);
    msg.insert("token", "");

    beginResetModel();
    m_json.push_front(msg);
    if(!unActive)
        m_currentRow = uuid;
    else
        setCountUnReadMessage(uuid, true);
    endResetModel();

    emit getStatusAddedUser(uuid);
    emit currentRowChanged();

}

QModelIndex SelectedUsersModel::item(const QString &uuid, int col)
{
    for (int i = 0; i < rowCount(); ++i) {
        auto currIndex = index(i, 0);
        if (data(currIndex, Qt::UserRole).toString() == uuid) {
            return index(i, col);
        }
    }

    return QModelIndex();
}


void SelectedUsersModel::removeRow(const QString& uuid)
{   
    for(int i = 0; i < m_json.size(); ++i ){
        QJsonObject obj = m_json.at(i).toObject();
        if (obj["uuid"].toString() == uuid) {
            beginResetModel();
            m_json.removeAt(i);
            if (m_currentRow ==  uuid) {
                if(m_json.size() > 0)
                    m_currentRow = data(index(0,0), Qt::UserRole).toString();
                else
                    m_currentRow = "";
            }
            endResetModel();
            emit currentRowChanged();
            return;
        }
    }
}



QString SelectedUsersModel::jsonText() const {

    QJsonDocument doc;
    QJsonObject mainObj = QJsonObject();
    mainObj.insert("currentRow", m_currentRow);

    QJsonObject obj = QJsonObject();
    obj.insert("columns", _header);
    obj.insert("rows", m_json);

    mainObj.insert("chats",obj);

    doc.setObject(mainObj);

    return QString::fromStdString(doc.toJson().toStdString());

}

void SelectedUsersModel::setJsonText(const QString &source) {

    if(source.isEmpty())
        return;
    QJsonDocument doc = QJsonDocument::fromJson(source.toUtf8());
    if(doc.isNull())
        return;
    setJson(doc);

}

QString SelectedUsersModel::userUuid()
{
    return _user_uuid;
}

void SelectedUsersModel::setUserUuid(const QString &uuid)
{
    _user_uuid = uuid;
}

QString SelectedUsersModel::currentRow()
{
    return m_currentRow;
}


void SelectedUsersModel::setCurrentRow(const QString &uuid)
{
    beginResetModel();
    m_currentRow = uuid;
    resetCountUnReadMsg();
    endResetModel();
    emit currentRowChanged();
}

void SelectedUsersModel::setRowValue(QModelIndex &index, const QVariant &value)
{
    QJsonObject obj = getJsonObject( index );
    const QString& key = m_header[index.column()]["index"];
    //if( obj.contains( key ))
    //{
        //if(value.typeId() == QMetaType::QString){
        if(value.userType() == QMetaType::QString){
            QJsonObject _obj = m_json[index.row()].toObject();
            _obj[key] = value.toString();
            m_json[index.row()] = _obj;
        }else if(value.userType() == QMetaType::Int){
            QJsonObject _obj = m_json[index.row()].toObject();
            _obj[key] = value.toInt();
            m_json[index.row()] = _obj;
        }else if(value.userType() == QMetaType::Bool){
            QJsonObject _obj = m_json[index.row()].toObject();
            _obj[key] = value.toBool();
            m_json[index.row()] = _obj;
        }
    //}

}

int SelectedUsersModel::getColumnIndex(const QString &name) {

    auto names = roleNames();
    if (names.size() == 0)
        return 0;

    for (auto key : names.keys()) {

        if (names[key] == name.toUtf8()){
            return key - Qt::UserRole;
        }
    }

    return 0;

}

void SelectedUsersModel::saveDraft(const QString& uuid, const QString &source)
{

    QModelIndex ind = item(uuid, getColumnIndex("draft"));
    setRowValue(ind, QVariant(source));

}

QString SelectedUsersModel::getDraft()
{
    QModelIndex ind = item(m_currentRow, getColumnIndex("draft"));
    QJsonObject obj = getJsonObject( ind );
    QJsonValue v = obj[ "draft" ];
    return v.toString("");
}

void SelectedUsersModel::setCountUnReadMessage(const QString &uuid, bool noReset )
{

    if (!is_already_added(uuid))
        return;

    //увеличиваем счетчик не прочитанных сообщений
    if(uuid == m_currentRow || uuid.isEmpty())
        return;

    int count = 0;
    int col = getColumnIndex("unreadMessages");
    if(col == 0)
        return;
    QModelIndex ind = item(uuid, col);
    QJsonObject obj = getJsonObject( ind );
    //динамическое обновление если нет поля
    if (obj.count() == 3){ //временно
        obj = getJsonObject( item(uuid, 0) );
        //if (!obj.contains("unreadMessages"))
        obj.insert("unreadMessages", 1);
        if(!noReset)
            beginResetModel();
        m_json[ind.row()] = obj;
        if(!noReset)
            endResetModel();
        //qDebug() << "incsert field unreadMessages";
        return;
    }
    //

    QJsonValue v = obj[ "unreadMessages" ];
    if(!noReset)
        beginResetModel();
    if(!v.isDouble()){
        count++;
        setRowValue(ind, count);
    }else{
        int currCount = v.toInt();
        count++;
        setRowValue(ind, count + currCount);
    }
    if(!noReset)
        endResetModel();
}

void SelectedUsersModel::setUnReadMessages(const QString &resp)
{
    QJsonDocument doc = QJsonDocument::fromJson(resp.toUtf8());

    if(doc.isNull())
        return;


    QJsonObject obj = doc.object();

    if(!obj.contains("rows"))
        return;

    QJsonArray arr = obj.value("rows").toArray();

    beginResetModel();

    for (int i = 0; i < arr.size() ; ++i ) {
        QString uuid = arr[i].toObject().value("sender").toString();
        QModelIndex ind = item(uuid);
        if(uuid == data(ind, Qt::UserRole).toString()){
            int val = arr[i].toObject().value("unreadMessages").toInt();
            QModelIndex curr = index(ind.row(), getColumnIndex("unreadMessages"));
;           setRowValue(curr, val);
        }
    }

    endResetModel();
}

void SelectedUsersModel::resetStatusActiveUsers(const QString &resp)
{

    QJsonDocument doc = QJsonDocument::fromJson(resp.toUtf8());
    if(doc.isEmpty())
        return;

    QJsonArray arr = doc.array();

    if(arr.empty())
        return;

    beginResetModel();
    for(int i = 0 ; i < rowCount(); ++i){
        QModelIndex ind = index(i, getColumnIndex("active"));
        setRowValue(ind, false);
    }

    for (auto itr : arr ) {
        QJsonObject obj = itr.toObject();
        if(obj.contains("user_uuid")){
            for(int i = 0 ; i < rowCount(); ++i){
                QModelIndex ind = index(i, getColumnIndex("active"));
                if(obj["user_uuid"] == data(ind, Qt::UserRole).toString())
                    setRowValue(ind, true);
            }
        }
    }
    endResetModel();
}

bool SelectedUsersModel::is_already_added(const QString &uuid) {

    for (int i = 0; i < m_json.size(); ++i) {
        QModelIndex ind = index(i, 0);
        if (data(ind, Qt::UserRole).toString() == uuid)
            return true;
    }

    return false;
}

bool SelectedUsersModel::isAlreadyAdded(const QString &uuid) {
    return is_already_added(uuid);
}

void SelectedUsersModel::setStatusUser(const QString &resp, bool value)
{
    QJsonDocument doc = QJsonDocument::fromJson(resp.toUtf8());
    if(doc.isNull())
        return;

    QJsonObject obj = doc.object();
    if(obj.empty())
        return;
    if(!obj.contains("uuid_user"))
        return;
    QString uuid = obj.value("uuid_user").toString();
    if(uuid == _user_uuid){
        return;
    }
    for(int i = 0 ; i < rowCount(); ++i){
        QModelIndex ind = index(i, getColumnIndex("active"));
        if(uuid == data(ind, Qt::UserRole).toString()){
            beginResetModel();
            setRowValue(ind, value);
            endResetModel();
            break;
        }
    }
}

void SelectedUsersModel::setStatusUser(const QString &resp)
{
    QJsonDocument doc = QJsonDocument::fromJson(resp.toUtf8());
    if(doc.isNull())
        return;

    QJsonObject obj = doc.object();
    if(obj.empty())
        return;
    if(!obj.contains("uuid_user"))
        return;
    QString uuid = obj.value("uuid_user").toString();
    if(uuid == _user_uuid){
        return;
    }
    bool value = obj.value("active").toBool();

    for(int i = 0 ; i < rowCount(); ++i){
        QModelIndex ind = index(i, getColumnIndex("active"));
        if(uuid == data(ind, Qt::UserRole).toString()){
            beginResetModel();
            setRowValue(ind, value);
            endResetModel();
            break;
        }
    }
}

void SelectedUsersModel::updateUserData(const QString &resp)
{
    QJsonDocument doc = QJsonDocument::fromJson(resp.toUtf8());
    if(doc.isNull())
        return;

    QJsonObject obj = doc.object();
    if(obj.empty())
        return;
    if(!obj.contains("uuid_user"))
        return;
    QString uuid = obj.value("uuid_user").toString();
    if(uuid == _user_uuid){
        return;
    }

    int row = -1;

    for (int i = 0; i < m_json.size(); ++i) {
        QModelIndex ind = index(i, 0);
        if (data(ind, Qt::UserRole).toString() == uuid){
            row = i;
            break;
        }
    }

    if(row == -1)
        return;


   beginResetModel();
   if(obj.contains("status")){
       QModelIndex ind = index(row, getColumnIndex("active"));
       setRowValue(ind, obj.value("status").toBool());
   }
   if(obj.contains("unreadMessages")){
       QModelIndex ind = index(row, getColumnIndex("unreadMessages"));
       setRowValue(ind, obj.value("unreadMessages").toInt());
   }

   endResetModel();

}

void SelectedUsersModel::setToken(const QString &token)
{
    QModelIndex ind = item(m_currentRow, getColumnIndex("token"));
    setRowValue(ind, token);
}

QString SelectedUsersModel::getCurrentToken()
{
    QModelIndex ind = item(m_currentRow, getColumnIndex("token"));
    QJsonObject obj = getJsonObject( ind );
    return obj["token"].toString();
}

