//
// Created by arcady on 10.11.2021.
//
#ifdef _WINDOWS
#pragma warning(disable:4100)
#endif

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
    _rowsIcon = QIcon();
    m_rowIcon = {};
    m_rowKeys = {};
    m_fmtText = {};
}

bool QJsonTableModel::setJson(const QJsonDocument &json)
{

    if (json.isNull()) {
        return false;
    }

    _header = json.object().value("columns").toArray();
    m_header.clear();

    int i = 0;
    foreach (const auto& itr , _header) {
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
    m_rowKeys.clear();
    m_rowKeys.resize(array.size());
    m_rowIcon.clear();
    m_fmtText.clear();
    endResetModel();
    return true;
}

QString QJsonTableModel::fromBase64(const QString &str)
{
    QString s = str.trimmed();
    QRegularExpression re("^[a-zA-Z0-9\\+/]*={0,3}$");
    bool isBase64 = (s.length() % 4 == 0) && re.match(s).hasMatch();
    if(!isBase64)
       return str;

    //QString result;
    try {
        return QByteArray::fromBase64(str.toUtf8());
    }  catch (std::exception&) {
        return str;
    }
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
        case Qt::Horizontal:{
            QString hData = m_header[section]["title"];
            if(hData.left(5) == "Empty")
                return QVariant();
            else{
                auto alias = m_colAliases.find(hData);
                if(alias != m_colAliases.end()){
                    return alias.value();
                }else
                    return hData;
            }
        }
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
        QHash<int, QByteArray> roles = roleNames();
        QString key = roles[role]; //QString::fromStdString(roleNames()[role].toStdString());
        if( obj.contains( key ))
        {
            QJsonValue v = obj[ key ];

            if( v.isString() )
            {
                auto itr = m_fmtText.find(index.column());
                if(itr != m_fmtText.end()){
                    if(itr.value() == "base64"){
                        return fromBase64(v.toString());
                    }else
                        return v.toString();
                }else
                    return v.toString();
            }
            else if( v.isDouble() )
            {               
                if(v.toDouble() != 0)
                    return QString::number( v.toDouble() );
                else
                    return QVariant();
            }
            else if( v.isBool() )
            {
                //return QString::number( v.toBool());
                return v.toBool();
            }
        }
    }if ( role == Qt::DecorationRole ) {
        if(index.column() == 0){
            QIcon _ico = _rowsIcon;
            if(!_deletionMarkRowsIcon.isNull()){
                 QJsonObject obj = getJsonObject( index );
                 if( obj.contains( "deletionMark" )){
                    QJsonValue v = obj[ "deletionMark" ];
                    if(v.isBool()){
                        if(v.toBool())
                           _ico = _deletionMarkRowsIcon;
                    }else if(v.isDouble()){
                        if(v.toInt() > 0)
                          _ico = _deletionMarkRowsIcon;
                    }
                 }
            }
            auto pair = qMakePair(index.row(), index.column());
            auto iter = m_rowIcon.constFind(pair);
            if(iter !=  m_rowIcon.end()){
                return iter.value();////m_rowIcon[pair];
            }else{
                if(!_ico.isNull())
                    return _ico;
            }
        }else{
            auto pair = qMakePair(index.row(), index.column());
            auto iter = m_rowIcon.constFind(pair);
            if(iter !=  m_rowIcon.end()){
                return iter.value();////m_rowIcon[pair];
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
                    auto itr = m_fmtText.find(index.column());
                    if(itr != m_fmtText.end()){
                        if(itr.value() == "base64"){
                            return fromBase64(v.toString());
                        }else
                            return v.toString();
                    }else
                        return v.toString();
                }
                else if( v.isDouble() )
                {
                    //return QString::number( v.toDouble() );
                    if(v.toDouble() != 0)
                        return QString::number( v.toDouble() );
                    else
                        return QVariant();
                }else if( v.isBool() )
                {
                    //return QString::number( v.toBool());
                    return v.toBool();
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
        return -1;

    foreach (const auto& key , names.keys()) {

        if (names[key] == name.toUtf8()){
            return key - Qt::UserRole;
        }
    }

    return -1;

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

void QJsonTableModel::setColumnAliases(const QMap<QString, QString> &columnAliases)
{
    m_colAliases = columnAliases;
}

void QJsonTableModel::reset() {
    beginResetModel();
    endResetModel();
}

void QJsonTableModel::setRowsIcon(const QIcon &ico)
{
    _rowsIcon = ico;
}

void QJsonTableModel::setDeletionMarkRowsIcon(const QIcon &ico)
{
    _deletionMarkRowsIcon = ico;
}

void QJsonTableModel::setIcon(const QModelIndex& index, const QIcon &ico)
{
    m_rowIcon.insert(qMakePair(index.row(), index.column()), ico);
}

void QJsonTableModel::setRowKey(int row, const QPair<QString, QString> &key)
{
    if(row < m_rowKeys.size() && row >= 0)
        m_rowKeys[row] = key;
}

QPair<QString, QString> QJsonTableModel::rowKey(int index) const
{
    if(index < m_rowKeys.size() && index >= 0)
        return m_rowKeys[index];
    else
        return qMakePair("","");
}

int QJsonTableModel::row(const QPair<QString, QString>& key)
{
    return m_rowKeys.indexOf(key);
}

QJsonObject QJsonTableModel::getRowObject(int row)
{
    if(row < m_json.size())
        return m_json[row].toObject();

    return QJsonObject();
}

void QJsonTableModel::updateRow(const QJsonObject &obj, int row)
{
    if(row >= m_json.size())
        return;

    m_json[row] = obj;

}

void QJsonTableModel::moveUp(int row)
{
    if(row - 1 <  0)
        return;

    int step = row-1;

    QJsonObject f =  m_json[row].toObject();
    QJsonObject l =  m_json[step].toObject();


     m_json[row] = l;
     m_json[step] = f;

     if(row <  m_rowKeys.size()-2){
         QPair<QString, QString> fp = m_rowKeys[row];
         QPair<QString, QString> lp = m_rowKeys[step];
         m_rowKeys[row] = lp;
         m_rowKeys[step] = fp;
     }

     QMap<QPair<int,int>, QIcon> first;
     QMap<QPair<int,int>, QIcon> second;

     for (auto itr = m_rowIcon.begin(); itr != m_rowIcon.end(); ++itr) {
         if(itr.key().first == row){
             first.insert(itr.key(),  itr.value());
         }else if(itr.key().first == step){
             second.insert(itr.key(),  itr.value());
         }
     }

     for (auto itr = first.constBegin(); itr != first.constEnd(); ++itr) {
         m_rowIcon.erase(m_rowIcon.constFind(itr.key()));
     }

     for (auto itr = second.constBegin(); itr != second.constEnd(); ++itr) {
         m_rowIcon.erase(m_rowIcon.constFind(itr.key()));
     }

     for (auto itr = first.constBegin(); itr != first.constEnd(); ++itr) {
         QPair<int,int> fp = qMakePair(step, itr.key().second);
         m_rowIcon.insert(fp, itr.value());
     }
     for (auto itr = second.constBegin(); itr != second.constEnd(); ++itr) {
         QPair<int,int> lp = qMakePair(row, itr.key().second);
         m_rowIcon.insert(lp, itr.value());
     }
}

void QJsonTableModel::moveDown(int row)
{

    if(row >  m_json.size()-2)
        return;

    int step = row+1;

    QJsonObject f =  m_json[row].toObject();
    QJsonObject l =  m_json[step].toObject();


     m_json[row] = l;
     m_json[step] = f;

     if(row <  m_rowKeys.size()-2){
         QPair<QString, QString> fp = m_rowKeys[row];
         QPair<QString, QString> lp = m_rowKeys[step];
         m_rowKeys[row] = lp;
         m_rowKeys[step] = fp;
     }

     QMap<QPair<int,int>, QIcon> first;
     QMap<QPair<int,int>, QIcon> second;

     for (auto itr = m_rowIcon.begin(); itr != m_rowIcon.end(); ++itr) {
         if(itr.key().first == row){
             first.insert(itr.key(),  itr.value());
         }else if(itr.key().first == step){
             second.insert(itr.key(),  itr.value());
         }
     }

     for (auto itr = first.constBegin(); itr != first.constEnd(); ++itr) {
         m_rowIcon.erase(m_rowIcon.constFind(itr.key()));
     }

     for (auto itr = second.constBegin(); itr != second.constEnd(); ++itr) {
         m_rowIcon.erase(m_rowIcon.constFind(itr.key()));
     }

     for (auto itr = first.constBegin(); itr != first.constEnd(); ++itr) {
         QPair<int,int> fp = qMakePair(step, itr.key().second);
         m_rowIcon.insert(fp, itr.value());
     }
     for (auto itr = second.constBegin(); itr != second.constEnd(); ++itr) {
         QPair<int,int> lp = qMakePair(row, itr.key().second);
         m_rowIcon.insert(lp, itr.value());
     }
}

void QJsonTableModel::setFormatColumn(int column, const QString &fmt)
{
    m_fmtText.insert(column, fmt);
}

void QJsonTableModel::removeRow(int row)
{
    if(row < rowCount()){
        m_json.removeAt(row);
        auto iter = m_fmtText.constFind(row);
        if(iter != m_fmtText.constEnd())
            m_fmtText.erase(iter);

        m_rowKeys.removeAt(row);

        for(int i = 0; i < _header.count(); ++i){
            auto pr = qMakePair(row, i);
            auto iterPr = m_rowIcon.constFind(pr);
            if(iterPr != m_rowIcon.constEnd())
                m_rowIcon.erase(iterPr);
        }

        reset();
    }

}

void QJsonTableModel::addRow(const QJsonObject &row)
{
    auto obj = QJsonObject();
    foreach(auto key, _header){
        auto val = row.find(key.toString());
        if(val != row.end()){
            obj.insert(key.toString(), val.value());
        }
    }    
    m_json.append(obj);
    reset();
    m_rowKeys.resize(m_json.size());
}
