#include "../include/messagelistmodel.h"
#include <QJsonObject>
#include <QString>
#include <iws_client.h>

MessageListModel::MessageListModel(QObject * parent )
    : QAbstractTableModel( parent )
{


}


QVariant MessageListModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int MessageListModel::rowCount(const QModelIndex &parent ) const
{
    return m_arrMsg[m_currentRecipient].size();
}


int MessageListModel::columnCount(const QModelIndex &parent ) const
{
    return m_header.size();
}


QJsonObject MessageListModel::getJsonObject( const QModelIndex &index ) const
{
    const QJsonValue& value = m_arrMsg[m_currentRecipient][index.row() ];
    return value.toObject();
}

QString MessageListModel::getRoleName(int role) const
{
    auto names = roleNames();
    if (names.size() == 0)
        return 0;

    for (auto key : names.keys()) {

        if (key == role){
            return QString::fromStdString(names[key].toStdString());
        }
    }

    return "";
}

QVariant MessageListModel::data( const QModelIndex &index, int role ) const
{

    QString roleName = getRoleName(role);

    if (roleName == "FirstField"){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "FirstField" ))
        {
            return obj["FirstField"].toString();
        }

    }
    else if (roleName == "message"){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "message" ))
        {
            QJsonValue v = obj[ "message" ];
            QString contentType = obj["contentType"].toString();
            QString base64 = v.toString();

            QString msg = QString::fromStdString(IClient::base64_decode(base64.toStdString()));
            QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
            QJsonObject content = doc.object();

            QString message = content["message"].toString();
            QString result;

            if(!contentType.isEmpty()){
                if (contentType == "HTML") {
                    result = QString::fromStdString(QByteArray::fromBase64(message.toUtf8()).toStdString());
                }else
                    result = message;
            }else
                result = message;

            return result;
        }

    }
    else if (roleName == "date"){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "date" ))
        {
            int unixTime = obj["date"].toInt();
            QDateTime timestamp = QDateTime::fromSecsSinceEpoch(unixTime);
            return timestamp;
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

QHash<int, QByteArray> MessageListModel::roleNames() const
{

    QHash<int, QByteArray> names;

    for (int i = 0; i < m_header.size(); ++i) {
        const QString& key = m_header[i]["index"];
        names[Qt::UserRole + i] = key.toUtf8();
    }

    return names;
}

void MessageListModel::sendMessage(const QString &message)
{

    qDebug() << "Send message: " << message;


}

void MessageListModel::setUserUuid(QString uuid)
{
    m_userUuid = uuid;
}


QString MessageListModel::userUuid()
{
    return m_userUuid;
}


void MessageListModel::clearRows()
{
    beginResetModel();
    m_arrMsg[m_currentRecipient] = QJsonArray();
    endResetModel();
}

void MessageListModel::addDocument(QJsonDocument doc, const QString& uuid)
{
    m_arrMsg.insert(uuid, doc.array());
}

void MessageListModel::addDocument(const QString& json){

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if(doc.isNull())
        return;


    QJsonObject obj = doc.object();

    if(m_header.count() == 0){
        _header = obj.value("columns").toArray();
        m_header.clear();

        int i = 0;
        for (auto itr : _header) {
            QString column = itr.toString();
            m_header.push_back( MessageListModel::Heading( { {"title",column},    {"index",column} }) );
            i++;
        }
    }


    if(!obj.contains( "uuid_recipient" ))
        return;

    QString uuid_recipient = obj.value("uuid_recipient").toString();
    QJsonArray mesgArr = obj.value("rows").toArray();

    m_Token = obj.value("token").toString();

    auto itr = m_arrMsg.find(uuid_recipient);
    if (itr != m_arrMsg.end()){
        m_arrMsg.erase(itr);
    }
     m_arrMsg.insert(uuid_recipient, mesgArr);

     beginResetModel();
     m_currentRecipient = uuid_recipient;
     endResetModel();

     emit tokenChanged(m_Token);
}

void MessageListModel::setDocument(const QString& uuid)
{
    if(uuid.isEmpty())
        return;

    if (uuid != m_currentRecipient) {
        //
        auto itr = m_arrMsg.find(uuid);

        if(itr == m_arrMsg.end()){
            emit getMessagesForRecipient(uuid);
        }
        else {
            beginResetModel();
            m_currentRecipient = uuid;
            endResetModel();
        }


    }
}


void MessageListModel::remove(const QString& uuid)
{
    beginResetModel();

    if (uuid == m_currentRecipient)
        m_currentRecipient = "";

    auto itr = m_arrMsg.find(uuid);

    if (itr != m_arrMsg.end())
        m_arrMsg.erase(itr);

    endResetModel();
}

void MessageListModel::addMessage(const QString& msg, const QString& uuid, const QString& recipient)
{

    QString message = QString::fromStdString(IClient::base64_decode(msg.toStdString()));

    QJsonObject _msg = QJsonDocument::fromJson(message.toUtf8()).object();

    QJsonArray rows = _msg.value("rows").toArray();

    qDebug() << recipient << " " << uuid << " " << m_userUuid << " " << currentRecipient();

    if(rows.size() > 0){
        auto itr = m_arrMsg.find(recipient);
        if(itr != m_arrMsg.end()){
            beginResetModel();
            m_arrMsg[recipient].push_front(rows[0].toObject());
            endResetModel();
        }
    }

}

QString MessageListModel::getCurrentChatHash() const
{
    //QString roomHash = IClient::get
}

QString MessageListModel::currentRecipient()
{
    return m_currentRecipient;
}

void MessageListModel::setCurrentRecipient(const QString &uuid)
{
    m_currentRecipient = uuid;
    setDocument(uuid);
}
