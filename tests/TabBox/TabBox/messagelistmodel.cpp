#include "messagelistmodel.h"
#include <QJsonObject>
#include <QString>

#include <sstream>
#include <string>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

const std::string base64_padding[] = {"", "==", "="};

static std::string base64_decode(const std::string &s) {

    namespace bai = boost::archive::iterators;

    std::stringstream os;

    typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;

    unsigned int size = s.size();

    // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
    if (size && s[size - 1] == '=') {
        --size;
        if (size && s[size - 1] == '=') --size;
    }
    if (size == 0) return std::string();

    std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
              std::ostream_iterator<char>(os));

    return os.str();
}

static std::string base64_encode(const std::string &s) {
    namespace bai = boost::archive::iterators;

    std::stringstream os;

    // convert binary values to base64 characters
    typedef bai::base64_from_binary
    // retrieve 6 bit integers from a sequence of 8 bit bytes
            <bai::transform_width<const char *, 6, 8>> base64_enc; // compose all the above operations in to a new iterator

    std::copy(base64_enc(s.c_str()), base64_enc(s.c_str() + s.size()),
              std::ostream_iterator<char>(os));

    os << base64_padding[s.size() % 3];
    return os.str();
}

MessageListModel::MessageListModel( const MessageListModel::Header& header, QObject * parent )
    : QAbstractTableModel( parent )
    , m_header( header )
{

}

//bool MessageListModel::setJson(QJsonDocument* json)
//{
//    //mDocs.insert(mDocs.size() + 1, json);
//    beginResetModel();
//    m_json = array;
//    endResetModel();
//    return setJson( json->array() );
//}

//bool MessageListModel::setJson(const QJsonArray& array )
//{
//    beginResetModel();
//    m_json = array;
//    endResetModel();
//    return true;
//}

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
    return m_arrMsg[currentIndex].size();
}

int MessageListModel::columnCount(const QModelIndex &parent ) const
{
    return m_header.size();
}


QJsonObject MessageListModel::getJsonObject( const QModelIndex &index ) const
{
    //const QJsonValue& value = m_json[index.row() ];
    const QJsonValue& value = m_arrMsg[currentIndex][index.row() ];
    return value.toObject();
}

QVariant MessageListModel::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::UserRole){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "FirstField" ))
        {
            return obj["FirstField"].toString();
        }

    }
    else if (role == Qt::UserRole + 5){
        QJsonObject obj = getJsonObject( index );
        if( obj.contains( "message" ))
        {
            QJsonValue v = obj[ "message" ];
            QString contentType = obj["contentType"].toString();
            QString base64 = v.toString();

            QString msg = QString::fromStdString(base64_decode(base64.toStdString()));
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
    else if (role == Qt::UserRole + 4){
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

    //qDebug() << "Send message: " << message;


}

void MessageListModel::setUserUuid(QString uuid)
{
    m_userUuid = uuid;
}

void MessageListModel::setCompanionUuid(QString uuid)
{
    m_companionUuid = uuid;
}

QString MessageListModel::userUuid()
{
    return m_userUuid;
}

QString MessageListModel::companionUuid()
{
    return m_companionUuid;
}

void MessageListModel::clearRows()
{
    beginResetModel();
    m_arrMsg[currentIndex] = QJsonArray();
    endResetModel();
}

void MessageListModel::addDocument(QJsonDocument doc, int itemIndex)
{
    m_arrMsg.insert(itemIndex, doc.array());
}

void MessageListModel::setDocument(int index)
{
    if (index != currentIndex) {
        beginResetModel();
        currentIndex = index;
        endResetModel();
    }
}

void MessageListModel::remove(int index)
{
    if (index == currentIndex)
        currentIndex = 0;

    auto itr = m_arrMsg.find(index);
    beginResetModel();
    if (itr != m_arrMsg.end())
        m_arrMsg.erase(itr);
    endResetModel();
}


void MessageListModel::onNewMessage(QString message)
{

    std::string jsonMsg = "{"
                   "\"uuid\": \"" + m_userUuid.toStdString() + "\","
                   "\"name\": \"Борисоглебский Аркадий\","
                   "\"uuid_channel\": \"7091f153-2d73-406a-a735-a3a31b67422b\","
                   "\"message\": \"" + message.toUtf8().toBase64().toStdString() + "\","
                   "\"uuid_form\": \"e7b5a692-6277-4299-912d-52ef42bb5dbb\","
                   "\"app_name\": \"unknown\","
                   "\"hash\": \"\","
                   "\"command\": \"message\","
                   "\"result\": \"ok\","
                   "\"role\": \"user\","
                   "\"user_uuid\": \"00000000-0000-0000-0000-000000000000\""
                   "}";

    QString sz_msg = QString::fromStdString(base64_encode(jsonMsg));

    QJsonObject msg = QJsonObject(); //doc.object();


    msg.insert("FirstField", "f3ccb2f2-d431-11e9-ab42-08606e7d17fa");
    msg.insert("Ref", "d3db7a39-c2b1-4ccb-ab67-a4f82f85a25d");
    msg.insert("SecondField", "d81ade18-fb07-11e2-b8bf-08606e7d17fa");
    msg.insert("_id", 0);
    msg.insert("date", 1630075939);
    msg.insert("message", sz_msg);
    msg.insert("token", "036a82ff52b0e5725e202ff17a5f82b1a1226141");
    msg.insert("contentType", "HTML");

    beginResetModel();
    m_arrMsg[currentIndex].push_front(msg);
    endResetModel();
}
