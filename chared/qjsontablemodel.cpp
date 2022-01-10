//
// Created by arcady on 10.11.2021.
//

#include "qjsontablemodel.h"

#include <QJsonObject>
#include <QString>
#include <QFileInfo>
#include <QDir>


QJsonTableModel::QJsonTableModel(QObject *parent)
        : QAbstractTableModel(parent)
{
    Header header;
    header.push_back( Heading( { {"title","title"},    {"index","title"} }) );
    m_header = header;
    m_json = QJsonArray();
    _header = QJsonArray();
}

bool QJsonTableModel::setJson(const QJsonDocument &json)
{

    if (json.isNull()) {
        return false;
    }

    _header = json.object().value("columns").toArray();
    m_header.clear();

    int i = 0;
    for (auto itr : _header) {
        QString column = itr.toString();
        m_header.push_back( QJsonTableModel::Heading( { {"title",column},    {"index",column} }) );
        i++;
    }

    auto _rows = json.object().value("rows").toArray();
    setJson( _rows );

    return true;
}

bool QJsonTableModel::setJson( const QJsonArray& array )
{
    beginResetModel();
    m_json = array;
    endResetModel();
    return true;
}

QVariant QJsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int QJsonTableModel::rowCount(const QModelIndex &parent ) const
{
    return m_json.size();
}

int QJsonTableModel::columnCount(const QModelIndex &parent ) const
{
    return m_header.size();
}

QJsonObject QJsonTableModel::getJsonObject( const QModelIndex &index ) const
{
    const QJsonValue& value = m_json[index.row() ];
    return value.toObject();
}

QVariant QJsonTableModel::data( const QModelIndex &index, int role ) const
{
    if (m_json.size() == 0)
        QVariant();

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
            else if( v.isBool() )
            {
                return QString::number( v.toBool());
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

QHash<int, QByteArray> QJsonTableModel::roleNames() const
{
    QHash<int, QByteArray> names;

    if (m_header.isEmpty())
        return names;

    for (int i = 0; i < m_header.size(); ++i) {
        const QString& key = m_header[i]["index"];
        int index = Qt::UserRole + i;
        names[index] = key.toUtf8();
    }

    return names;
}

int QJsonTableModel::getColumnIndex(const QString &name) {

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

QString QJsonTableModel::jsonText() const {

    QJsonDocument doc;
    QJsonObject obj = QJsonObject();
    obj.insert("columns", _header);
    obj.insert("rows", m_json);

    doc.setObject(obj);

    return QString::fromStdString(doc.toJson().toStdString());

}

void QJsonTableModel::setJsonText(const QString &source) {

    if(source.isEmpty())
        return;
    QJsonDocument doc = QJsonDocument::fromJson(source.toUtf8());
    if(doc.isNull())
        return;
    setJson(doc);

}

void QJsonTableModel::reset() {
    beginResetModel();
    endResetModel();
}
