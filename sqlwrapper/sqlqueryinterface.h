//
// Created by admin on 04.06.2022.
//

#ifndef WS_SOLUTION_SQLQUERYINTERFACE_H
#define WS_SOLUTION_SQLQUERYINTERFACE_H
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlQuery>

enum QBSqlCommand{
    QSqlInsert = 0,
    QSqlUpdate,
    QSqlDelete,
    QSqlGet
};

enum QBSqlTypeOfComparison{
    QEquals = 0,
    QNo_Equals,
    QMore,
    QMore_Or_Equal,
    QLess_Or_Equal,
    QLess,
    QOn_List,
    QNot_In_List
};

enum bFieldType{
    qVariant = 0,
    qByteArray
};

struct QBSqlValue{
    QVariant value;
    QByteArray data;
    bFieldType type;
    QBSqlValue(){};
};

class QBSqlQuery{
public:

    explicit QBSqlQuery();
    explicit QBSqlQuery(QBSqlCommand command, const QString& tableName);

    void add_field(const QJsonObject& val, bFieldType field_type);

    void add_where(const QJsonObject& val, QBSqlTypeOfComparison typeOfComparison);

    void set_where_sample(const QString& str);

    void add_field_is_exists(const QJsonObject& field);

    QString to_json() const;

    void fromJson(const QString& json);

    QSqlQuery query(const QSqlDatabase& db);

private:
    QBSqlCommand _command;
    QList<QJsonObject> _fields;
    QList<QJsonObject> _fieldsIsExists;
    QString _whereSample;
    QList<QJsonObject> _where;
    QString _table;

    QString bindQueryInsert(QList<QBSqlValue>& values) const;
    QString bindQueryUpdate(QList<QBSqlValue>& values) const;
    QString bindQueryDelete() const;
    QString bindQueryGet() const;

    QString comparison(QBSqlTypeOfComparison typeOfComparison) const;
};
#endif //WS_SOLUTION_SQLQUERYINTERFACE_H

