//
// Created by admin on 04.06.2022.
//
#include "sqlqueryinterface.h"
#include <iostream>

//QBSqlValue::QBSqlValue(QBSqlTypeOfComparison _typeOfComparison, QJsonObject _field){
//    typeOfComparison = _typeOfComparison;
//    field = std::move(_field);
//}


QBSqlQuery::QBSqlQuery()
{

}

QBSqlQuery::QBSqlQuery(QBSqlCommand command, const QString &tableName)
{
    _command = command;
    _table = tableName;
}

void QBSqlQuery::add_field(const QJsonObject &field, bFieldType field_type)
{
    QJsonObject obj = QJsonObject();
    obj.insert("type", field_type);
    obj.insert("value", field);
    _fields.push_back(obj);
}

void QBSqlQuery::add_where(const QJsonObject &val, QBSqlTypeOfComparison typeOfComparison)
{
    auto obj_where = QJsonObject();
    obj_where.insert("typeOfComparison", typeOfComparison);
    obj_where.insert("value", val);
    _where.append(obj_where);
}

void QBSqlQuery::set_where_sample(const QString &str)
{
    _whereSample = str;
}

void QBSqlQuery::add_field_is_exists(const QJsonObject &field)
{
    _fieldsIsExists.append(field);
}

QString QBSqlQuery::to_json() const
{

    auto result = QJsonDocument();
    auto objMain = QJsonObject();

    objMain.insert("command", _command);
    objMain.insert("table", _table);
    objMain.insert("whereSample", _whereSample);

    auto arr_fields = QJsonArray();

    for(auto itr : _fields){
        arr_fields.append(itr);
    }
    objMain.insert("fields", arr_fields);

    auto arr_fieldsIsExists = QJsonArray();

    for(auto itr : _fieldsIsExists){
        arr_fieldsIsExists.append(itr);
    }
    objMain.insert("fieldsIsExists", arr_fieldsIsExists);

    auto arr_where = QJsonArray();
    for(auto itr : _where){
        arr_where.append(itr);
    }
    objMain.insert("where", arr_where);

    result.setObject(objMain);

    return result.toJson();
}

void QBSqlQuery::fromJson(const QString &json)
{
    auto doc = QJsonDocument::fromJson(json.toUtf8());
    auto mObj = doc.object();

    _command = (QBSqlCommand)mObj.value("command").toInt();
    _table = mObj.value("table").toString();
    _whereSample = mObj.value("whereSample").toString();

    auto arr_fields = mObj.value("fields").toArray();

    for(auto itr : arr_fields){
        auto obj = itr.toObject();
        _fields.append(obj);
    }

    auto arr_fieldsIsExists = mObj.value("fieldsIsExists").toArray();

    for(auto itr : arr_fieldsIsExists){
        auto obj = itr.toObject();
        _fieldsIsExists.append(obj);
    }

    auto arr_where = mObj.value("where").toArray();

    for(auto itr : arr_where){
        auto obj = itr.toObject();
        _where.append(obj);
    }
}

QSqlQuery QBSqlQuery::query(const QSqlDatabase& db)
{
    QString _query;
    QList<QBSqlValue> values;
    QSqlQuery sql(db);

    if(_command == QSqlInsert){
        _query = bindQueryInsert(values);
    }else if(_command == QSqlUpdate){
        _query = bindQueryUpdate(values);
    }else if(_command == QSqlDelete){
        _query = bindQueryDelete();
    }else if(_command == QSqlGet){
        _query = bindQueryGet();
    }

    sql.prepare(_query);

    for (auto itr : values) {
        if(itr.type == qVariant){
            sql.addBindValue(itr.value);
            qDebug() << itr.value.toString();
        }else
            sql.addBindValue(itr.data);
    }

    return sql;
}

QString QBSqlQuery::bindQueryInsert(QList<QBSqlValue>& values) const
{

    QString query;
    QString into = "\n(";
    QString values_ = "\n(";
    query = "INSERT INTO ";
    query.append("dbo." + _table);
    for (auto iter = _fields.begin(); iter != _fields.end(); iter++) {
        bFieldType type = (bFieldType)iter->value("type").toInt();
        QJsonObject val = iter->value("value").toObject();
        into.append(val.value("name").toString());
        if (iter != --_fields.end())
            into.append(",\n");

        QBSqlValue item = QBSqlValue();
        item.type = type;
        QString v = val.value("value").toString();
        if(type == bFieldType::qVariant){
           item.value = v;
        }else
           item.data = QByteArray::fromBase64(v.toUtf8());

        values.append(item);

        values_.append("?");
        if (iter != --_fields.end())
            values_.append(",\n");
    }

    into.append("\n)");
    values_.append("\n)");
    query.append(into);
    query.append("\nVALUES");
    query.append(values_);

    if(!_fieldsIsExists.empty()){
        QString _whereEx;

        for (auto iter = _fieldsIsExists.begin(); iter != _fieldsIsExists.end(); iter++) {
            _whereEx.append(iter->value("name").toString());
            auto vValue = iter->value("value");
            if (vValue.isString()){
                QString value = vValue.toString();
                _whereEx.append(" = '" + value + "'");
                if (iter != --_fieldsIsExists.end())
                    _whereEx.append(" AND\n");
            }else if (vValue.isDouble()){
                int res = vValue.toInt();
                QString value = QString::number(res);
                _whereEx.append(" = '" + value + "'");
                if (iter != --_fieldsIsExists.end())
                    _whereEx.append(" AND\n");
            }
        }

        QString query_ = QString("IF NOT EXISTS \n"
                                 "    (   SELECT  [Ref]\n"
                                 "        FROM    dbo.%1 \n"
                                 "        WHERE   %2 \n"
                                 "    )\n"
                                 "BEGIN\n").arg(_table, _whereEx);
        query_.append(query);
        query_.append("\nEND");
        query = query_;
    }

    query.append(";");

    return query;
}

QString QBSqlQuery::bindQueryUpdate(QList<QBSqlValue>& values) const
{
    QString query;
    QString _set = "\nSET ";
    query = "UPDATE ";
    query.append("dbo." + _table);
    for (auto iter = _fields.begin(); iter != _fields.end(); iter++) {
        bFieldType type = (bFieldType)iter->value("type").toInt();
        QJsonObject val = iter->value("value").toObject();
        _set.append("[" + val.value("name").toString() + "]");
        _set.append(" = ?"); //'" + value + "'");
        if (iter != --_fields.end())
            _set.append(",\n");

        QBSqlValue item = QBSqlValue();
        item.type = type;
        QString v = val.value("value").toString();
        if(type == bFieldType::qVariant){
           item.value = v;
        }else
           item.data = QByteArray::fromBase64(v.toUtf8());
        values.append(item);
    }

    query.append(_set);

    if(!_fieldsIsExists.empty()){
        QString _whereEx;

        for (auto iter = _fieldsIsExists.begin(); iter != _fieldsIsExists.end(); iter++) {
            _whereEx.append(iter->value("name").toString());
            auto vValue = iter->value("value");
            if (vValue.isString()){
                QString value = vValue.toString();
                _whereEx.append(" = '" + value + "'");
                if (iter != --_fieldsIsExists.end())
                    _whereEx.append(" AND\n");
            }else if (vValue.isDouble()){
                int res = vValue.toInt();
                QString value = QString::number(res);
                _whereEx.append(" = '" + value + "'");
                if (iter != --_fieldsIsExists.end())
                    _whereEx.append(" AND\n");
            }
        }
    }

    query.append("\nWHERE ");

    QString _whereEx;

    for (auto iter = _where.begin(); iter != _where.end(); iter++) {
        QBSqlTypeOfComparison type = (QBSqlTypeOfComparison)iter->value("typeOfComparison").toInt();
        QJsonObject val = iter->value("value").toObject();

        if(type == QBSqlTypeOfComparison::QNo_Equals || type == QBSqlTypeOfComparison::QNot_In_List){
             _whereEx.append(" NOT ");
        }

        QString cmp = comparison(type);

        _whereEx.append(val.value("name").toString());
        auto vValue = val.value("value");
        if (vValue.isString()){
            QString value = vValue.toString();
            _whereEx.append(cmp + " '" + value + "'");
            if (iter != --_where.end())
                _whereEx.append(" AND\n");
        }else if (vValue.isDouble()){
            int res = vValue.toInt();
            QString value = QString::number(res);
            _whereEx.append(cmp + " '" + value + "'");
            if (iter != --_where.end())
                _whereEx.append(" AND\n");
        }else if (vValue.isArray()){

        }
    }
    query.append(_whereEx);

    return query;
}

QString QBSqlQuery::bindQueryDelete() const
{
    QString query;
    query = "DELETE FROM ";
    query.append("dbo." + _table);
    query.append("\nWHERE ");

    QString _whereEx;

    for (auto iter = _where.begin(); iter != _where.end(); iter++) {
        QBSqlTypeOfComparison type = (QBSqlTypeOfComparison)iter->value("typeOfComparison").toInt();
        QJsonObject val = iter->value("value").toObject();

        if(type == QBSqlTypeOfComparison::QNo_Equals || type == QBSqlTypeOfComparison::QNot_In_List){
             _whereEx.append(" NOT ");
        }

        QString cmp = comparison(type);

        _whereEx.append(val.value("name").toString());
        auto vValue = val.value("value");
        if (vValue.isString()){
            QString value = vValue.toString();
            _whereEx.append(cmp + " '" + value + "'");
            if (iter != --_where.end())
                _whereEx.append(" AND\n");
        }else if (vValue.isDouble()){
            int res = vValue.toInt();
            QString value = QString::number(res);
            _whereEx.append(cmp + " '" + value + "'");
            if (iter != --_where.end())
                _whereEx.append(" AND\n");
        }else if (vValue.isArray()){

        }
    }
    query.append(_whereEx);
    return query;
}

QString QBSqlQuery::bindQueryGet() const
{
    QString query;
    query = "SELECT ";

    QString select = "";

    for (auto iter = _fields.begin(); iter != _fields.end(); iter++) {
        QJsonObject val = iter->value("value").toObject();
        QString name = val.value("name").toString();
        select.append(name.left(5) == "Empty" ? "NULL" : "[" + name + "]");
        QString v = val.value("value").toString();
        if(!v.isEmpty())
            select.append(" AS [" + v + "]");
        if (iter != --_fields.end())
            select.append(",\n");
    }

    query.append(select);
    query.append(" FROM dbo." + _table);

    if(_where.size() > 0){
        query.append("\nWHERE ");
        QString _whereEx;
        for (auto iter = _where.begin(); iter != _where.end(); iter++) {
            QBSqlTypeOfComparison type = (QBSqlTypeOfComparison)iter->value("typeOfComparison").toInt();
            QJsonObject val = iter->value("value").toObject();

            if(type == QBSqlTypeOfComparison::QNo_Equals || type == QBSqlTypeOfComparison::QNot_In_List){
                 _whereEx.append(" NOT ");
            }

            QString cmp = comparison(type);

            _whereEx.append(val.value("name").toString());
            auto vValue = val.value("value");
            if (vValue.isString()){
                QString value = vValue.toString();
                _whereEx.append(cmp + " '" + value + "'");
                if (iter != --_where.end())
                    _whereEx.append(" AND\n");
            }else if (vValue.isDouble()){
                int res = vValue.toInt();
                QString value = QString::number(res);
                _whereEx.append(cmp + " '" + value + "'");
                if (iter != --_where.end())
                    _whereEx.append(" AND\n");
            }else if (vValue.isArray()){

            }
        }
        query.append(_whereEx);
    }
    return query;
}

QString QBSqlQuery::comparison(QBSqlTypeOfComparison typeOfComparison) const
{
    QString result;


    if(typeOfComparison == QBSqlTypeOfComparison::QEquals || typeOfComparison == QBSqlTypeOfComparison::QNo_Equals)
        result.append(" = ");
    else if(typeOfComparison == QBSqlTypeOfComparison::QNot_In_List || typeOfComparison== QBSqlTypeOfComparison::QOn_List)
       result.append(" IN ");
    else if(typeOfComparison == QBSqlTypeOfComparison::QLess)
        result.append(" < ");
    else if(typeOfComparison == QBSqlTypeOfComparison::QMore)
        result.append(" > ");
    else if(typeOfComparison == QBSqlTypeOfComparison::QLess_Or_Equal)
        result.append(" <= ");
    else if(typeOfComparison == QBSqlTypeOfComparison::QMore_Or_Equal)
        result.append(" >= ");

    return result;
}


