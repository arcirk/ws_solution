#include "sqlinterface.h"
#include <QDir>
#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <iostream>
#include <QSqlRecord>
#include <QJsonArray>
#include <QSqlField>
#include <QSqlError>
#include "sqlqueryinterface.h"

SqlInterface::SqlInterface(QObject *parent)
    : QObject{parent}
{
    _driverType = "QODBC";
}

SqlInterface::~SqlInterface()
{
    if(db.isOpen())
        db.close();
}

void SqlInterface::setHost(const QString &value)
{
    _host = value;
}

QString SqlInterface::host() const
{
    return _host;
}

void SqlInterface::setSqlUser(const QString &value)
{
    _user = value;
}

QString SqlInterface::user() const
{
    return _user;
}

void SqlInterface::setSqlPwd(const QString &value)
{
    _pwd = value;
}

QString SqlInterface::pwd() const
{
    return _pwd;
}

void SqlInterface::setDatabaseName(const QString &value)
{
#ifdef _WINDOWS
    if(driver() == "QSQLITE")
        _databaseName = QCoreApplication::applicationDirPath() + QDir::separator() + value;
    else //if(driver() == "QODBC")
        _databaseName = value;
#else
    _databaseName = value;
#endif
}

QString SqlInterface::databaseName() const
{
    return _databaseName;
}

QString SqlInterface::driver(){
    return _driverType;
}

void SqlInterface::setDriver(const QString &value)
{
    _driverType = value;
}

bool SqlInterface::connect(const QString &driver)
{
    bool result = false;
    try {
        db = QSqlDatabase::addDatabase(driver, "arcirk");

        if (driver == "QSQLITE") {
            QString dbPath = QDir::fromNativeSeparators(databaseName());

            QFile dbFile(dbPath);
            if (!dbFile.exists()) {
                return false;
            }
            db.setDatabaseName(dbPath);
        } else if (driver == "QODBC") {
            db.setUserName(user());
            db.setPassword(pwd());
            db.setHostName(host());
            db.setDatabaseName(QString("DRIVER={SQL Server};"
                                       "SERVER=%1;Persist Security Info=true;"
                                       "uid=%2;pwd=%3").arg(host(), user(), pwd()));
//            db.setDatabaseName(QString("DRIVER={ODBC Driver 17 for SQL Server};"
//                                       "SERVER=%1;Persist Security Info=true;"
//                                       "uid=%2;pwd=%3").arg(host(), user(), pwd()));
//            db.setDatabaseName(QString("DRIVER={SQL Server Native Client 11.0};"
//                                       "SERVER=%1;Persist Security Info=true;"
//                                       "uid=%2;pwd=%3").arg(host(), user(), pwd()));
            db.setConnectOptions();
        }

        _driverType = driver;

        result = db.open();

        if(result){
            QSqlDatabase::removeDatabase("default");
            result = verifyDatabase();
//            if(result){
//                db.close();
//                db.setDatabaseName(QString("DRIVER={SQL Server};"
//                                           "SERVER=%1;Persist Security Info=true;"
//                                           "uid=%2;pwd=%3;DATABASE=%4").arg(host(), user(), pwd(), databaseName()));
//                db.setConnectOptions();
//                result = db.open();
//                std::cout << db.databaseName().toStdString() << std::endl;
//            }
        }

    }catch (std::exception& e){
        std::cerr << e.what() << std::endl;

    }
    return result;
}

bool SqlInterface::isOpen()
{
    return db.isOpen();
}

void SqlInterface::close()
{
    if(db.isOpen())
        db.close();
}

bool SqlInterface::verifyDatabase() {

    if(!isOpen() || _driverType != "QODBC" || databaseName().isEmpty())
        return false;

    QString query = QString("IF NOT EXISTS (SELECT * FROM sys.databases WHERE name = '%1')\n"
                            "BEGIN\n"
                            "CREATE DATABASE %1\n"
                            "END;").arg(databaseName());

    auto result = db.exec(query);

    query = QString("SELECT * FROM sys.databases WHERE name = '%1'").arg(databaseName());
    result = db.exec(query);

    bool isExists = false;

    while (result.next()){
        isExists = true;
        break;
    }
    if(isExists){
        db.exec(QString("USE [%1]").arg(databaseName()));
//        std::cout << databaseName().toStdString() << std::endl;
//        std::cout MM
        std::cout << QString("Connected database '%1'").arg(databaseName()).toStdString() << std::endl;
        return true;
    }

    return false;

}

int SqlInterface:: exec(const QString &query, QString& err) {

    err = "";
    int count = 0;
    try{
        if(!isOpen())
            return 0;

        db.exec("USE " + databaseName());


        auto result = db.exec(query);
        if(result.lastError().type() != QSqlError::NoError){
            err = result.lastError().text();
            std::cerr << "SqlInterface::exec: " << query.toStdString() <<  std::endl;;
            std::cerr << "SqlInterface::exec: " << "Ошибка SQL запроса : " << err.toStdString() << std::endl;
            return 0;
        }


        while (result.next()){
            count++;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }


    return count;

}

int SqlInterface::exec_qt(const QString &i_query, QString &err)
{

    auto bindQuery = QBSqlQuery();
    bindQuery.fromJson(i_query);

    QSqlQuery sql = bindQuery.query(db);
    bool res = sql.exec();

    if(sql.lastError().type() != QSqlError::NoError){
        err = sql.lastError().text();
        std::cout << "SqlInterface::exec_qt:query " << sql.lastQuery().toStdString() << std::endl;
    }

    return res;
}

int SqlInterface::exec_qt(const QString &i_query, QString &tableResult, QString &err)
{
    auto bindQuery = QBSqlQuery();
    bindQuery.fromJson(i_query);
    QSqlQuery m_query = bindQuery.query(db);

    bool res = m_query.exec();

    int rowCount = 0;

    if(m_query.lastError().type() != QSqlError::NoError){
        err = m_query.lastError().text();
        std::cout << "SqlInterface::exec_qt:query_error: " << m_query.lastQuery().toStdString() << std::endl;
    }else{

        int count = m_query.record().count();

        auto rows = QJsonArray();
        auto cols = QJsonArray();

        for (int i = 0; i < count; ++i) {
            QString key = m_query.record().fieldName(i);
            cols.push_back(key);
        }
        while (m_query.next()) {
            QJsonObject obj = QJsonObject();
            for (int i = 0; i < count; ++i) {
                QString key = m_query.record().fieldName(i);
                QVariant val = m_query.value(i);
                if (val.typeId() == QMetaType::QString)
                    obj.insert(key, val.toString().trimmed());
                else if (val.typeId() == QMetaType::Double)
                    obj.insert(key, val.toDouble());
                else if (val.typeId() == QMetaType::Int)
                    obj.insert(key, val.toInt());
                else if (val.typeId() == QMetaType::QByteArray)
                    obj.insert(key, QString(val.toByteArray().toBase64()));
            }

            rows.push_back(obj);
            rowCount++;
        }
        QJsonDocument doc = QJsonDocument();
        QJsonObject obj = QJsonObject();
        obj.insert("columns", cols);
        obj.insert("rows", rows);

        doc.setObject(obj);
        tableResult = doc.toJson();
    }

    return res;
}

bool SqlInterface::verifyTable(int tableIndex) {

    if(!isOpen() || _driverType != "QODBC" || databaseName().isEmpty())
        return false;

    db.exec(QString("USE [%1]").arg(databaseName()));

    QString vTable = QString("SELECT count([name]) as [rowcount] FROM sysobjects where  name = '%1'").arg(tables[tableIndex]);
    //std::cout << vTable.toStdString() << std::endl;
    auto sqlQuery = db.exec(vTable);
    bool isExists = false;
    while (sqlQuery.next()){
        if(sqlQuery.value(0).toInt() > 0){
            isExists = true;
            break;
        }
        //std::cout << "exists table name rowcount:" + QString::number(sqlQuery.value(0).toInt()).toStdString() << std::endl;
    }
    if(!isExists){
        QString query = queryTemplate(tableIndex);
        QString fields = tableFields(tableIndex);
        query.replace("_NEXT_FIELDS_", fields);

        db.exec(query);
        sqlQuery = db.exec(QString("SELECT * FROM sysobjects where  name = '%1'").arg(tables[tableIndex]));
        while (sqlQuery.next()){
            isExists = true;
            std::cout << QString("Created table '%1'").arg(tables[tableIndex]).toStdString() << std::endl;
            break;
        }
        if(isExists){
            setIndexes(tableIndex);
        }else{
            std::cerr << "error query: \n" << QString(query).toStdString() << std::endl;
        }
    }

    return isExists;

}

QString SqlInterface::queryTemplate(int tableIndex) const{
    const QString& tableName = tables[tableIndex];
    QString query = QString("CREATE TABLE [dbo].[%1](\n"
                    "[_id] [int] IDENTITY(1,1) NOT NULL,\n"
                    "[FirstField] [varchar](max) NULL,\n"
                    "[SecondField] [varchar](max) NULL,\n"
                    "[Ref] [char](36) NOT NULL,\n"
                    "_NEXT_FIELDS_\n"
                    "CONSTRAINT [PK_%1] PRIMARY KEY CLUSTERED\n"
                    "(\n"
                    "[_id] ASC\n"
                    ")WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]\n"
                    ") ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]").arg(tableName);
    return query;
}

QString SqlInterface::tableFields(int tableIndex) {
    const QString& tableName = tables[tableIndex];
    QString result = "";
    if(tableName == "Users"){
        result = "[hash] [char](1000) NOT NULL,\n"
                 "[role] [char](10) NULL,\n"
                 "[Performance] [varchar](max) NULL,\n"
                 "[channel] [char](36) NOT NULL,\n"
                 "[parent] [char](36) NULL,\n"
                 "[cache] [text] NULL,\n"
                 "[isGroup] [int] NOT NULL,"
                 "[block] [int] NOT NULL,"
                 "[allowСhat] [int] NOT NULL,"
                 "[deletionMark] [int] NOT NULL,"
                 "[FullName] [varchar](max) NULL,"
                 "[LastName] [varchar](max) NULL,"
                 "[FirstName] [varchar](max) NULL,"
                 "[Reporting] [varchar](max) NULL,";
    }else if(tableName == "Channels"){
        result = "[parent] [char](36) NOT NULL,\n"
                 "[cache] [text] NULL,";
    }else if(tableName == "Messages"){
        result = "[message] [text] NULL,\n"
                 "[token] [varchar](max) NOT NULL,\n"
                 "[date] [int] NOT NULL,\n"
                 "[contentType] [char](10) NULL,\n"
                 "[unreadMessages] [int] NULL,";
    }else if(tableName == "Subscribers"){
        result = "[channel] [char](36) NOT NULL,\n"
                 "[user_uuid] [char](36) NOT NULL,\n"
                 "[cache] [text] NULL,";
    }else if(tableName == "TechnicalInformation"){
        result = "[cache] [text] NULL,";
    }else if(tableName == "Containers"){
        result = "[cache] [text] NULL,\n"
                 "[data] [varbinary](max) NULL,"
                 "[subject] [varchar](max) NULL,"
                 "[issuer] [varchar](max) NULL,"
                 "[notValidBefore] [varchar](max) NULL,"
                 "[notValidAfter] [varchar](max) NULL,"
                 "[parentUser] [varchar](max) NULL,";
    }else if(tableName == "CertUsers"){
        result = "[cache] [text] NULL,\n"
                 "[uuid] [char](38) NULL,\n"
                 "[sid] [varchar](136) NULL,\n"
                 "[domain] [varchar](max) NULL,";
    }else if(tableName == "WSConf"){
        result = "[cache] [text] NULL,\n"
                 "[host] [varchar](max) NULL,\n"
                 "[port] [int] NULL,";
    }else if(tableName == "Servers"){
        result = "[cache] [text] NULL,"
                 "[ipadrr] [varchar](max) NULL,"
                 "[isServer] [int] NOT NULL,";
    }else if(tableName == "Certificates"){
        result = "[cache] [text] NULL,\n"
                 "[data] [varbinary](max) NULL,"
                 "[privateKey] [char](36) NULL"
                 "[subject] [varchar](max) NULL,"
                 "[issuer] [varchar](max) NULL,"
                 "[notValidBefore] [varchar](max) NULL,"
                 "[notValidAfter] [varchar](max) NULL,"
                 "[parentUser] [varchar](max) NULL,"
                 "[serial] [varchar](max) NULL,";
    }


    return result;
}

void SqlInterface::setIndexes(int tableIndex) {

    const QString& tableName = tables[tableIndex];
    if(tableName == "Users"){
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_hash]  DEFAULT ('0') FOR [hash]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_role]  DEFAULT ('user') FOR [role]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_channel]  DEFAULT ('00000000-0000-0000-0000-000000000000') FOR [channel]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_parent]  DEFAULT ('00000000-0000-0000-0000-000000000000') FOR [parent]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_isGroup]  DEFAULT ((0)) FOR [isGroup]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_block]  DEFAULT ((0)) FOR [block]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_allowСhat]  DEFAULT ((1)) FOR [allowСhat]");
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_deletionMark]  DEFAULT ((0)) FOR [deletionMark]");
    }else if(tableName == "Channels"){
        db.exec("ALTER TABLE [dbo].[Channels] ADD  CONSTRAINT [DF_Channels_Parent]  DEFAULT ('00000000-0000-0000-0000-000000000000') FOR [Parent]");
    }else if(tableName == "Messages"){
        db.exec("ALTER TABLE [dbo].[Messages] ADD  CONSTRAINT [DF_Messages_contentType]  DEFAULT ('HTML') FOR [contentType]");
        db.exec("ALTER TABLE [dbo].[Messages] ADD  CONSTRAINT [DF_Messages_unreadMessages]  DEFAULT ((0)) FOR [unreadMessages]");
    }else if(tableName == "Servers"){
        db.exec("ALTER TABLE [dbo].[Servers] ADD  CONSTRAINT [DF_Servers_isServer]  DEFAULT ((0)) FOR [isServer]");
    }

}

bool SqlInterface::verifyViews() {

    if(!isOpen() || _driverType != "QODBC" || databaseName().isEmpty())
        return false;

    QString vTable = "SELECT * FROM sysobjects where  name = 'UsersCatalog'";
    auto sqlQuery = db.exec(vTable);
    bool isExists = false;
    while (sqlQuery.next()){
        isExists = true;
        break;
    }
    QString query;
    if(!isExists){
        query = "CREATE VIEW [dbo].[UsersCatalog]\n"
                        "AS\n"
                        "SELECT FirstField, SecondField, Ref, Parent, 1 AS IsGroup\n"
                        "FROM  dbo.Channels\n"
                        "UNION\n"
                        "SELECT  FirstField, SecondField, Ref, channel AS Parent, 0 AS IsGroup\n"
                        "FROM dbo.Users";

        sqlQuery = db.exec(query);
        while (sqlQuery.next()){
            isExists = true;
            std::cout << "Created view 'UsersCatalog'" << std::endl;
            break;
        }
    }

    if(!isExists){
        std::cerr << "error create view: " << query.toStdString() << std::endl;
    }

    return isExists;
}

int SqlInterface::execute(const std::string &query, QString &table, QString &error, bool header) {

    int rowCount = 0;

    try {
        if (!isOpen())
            return 0;

        error = "no error";

        QSqlQuery m_query;
        try {
            m_query = db.exec(QString::fromStdString(query));
            if(m_query.lastError().type() != QSqlError::NoError){
                error = m_query.lastError().text();
                std::cerr << "SqlInterface::execute: " << query << std::endl;
                std::cerr << "SqlInterface::execute: " << "Ошибка SQL запроса : " << error.toStdString() << std::endl;
                std::cerr << m_query.lastError().databaseText().toStdString() << std::endl;
                std::cerr << m_query.lastError().driverText().toStdString() << std::endl;
                std::cerr << m_query.lastQuery().toStdString() << std::endl;
                return 0;
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            error = QString(e.what());
            return 0;
        }

        int count = m_query.record().count();

        auto rows = QJsonArray();
        auto cols = QJsonArray();

        for (int i = 0; i < count; ++i) {
            QString key = m_query.record().fieldName(i);
            cols.push_back(key);
        }
        while (m_query.next()) {
            QJsonObject obj = QJsonObject();
            for (int i = 0; i < count; ++i) {
                QString key = m_query.record().fieldName(i);
                QVariant val = m_query.value(i);
                if (val.typeId() == QMetaType::QString)
                    obj.insert(key, val.toString().trimmed());
                else if (val.typeId() == QMetaType::Double)
                    obj.insert(key, val.toDouble());
                else if (val.typeId() == QMetaType::Int)
                    obj.insert(key, val.toInt());
                else if (val.typeId() == QMetaType::QByteArray)
                    obj.insert(key, QString(val.toByteArray().toBase64()));
            }

            rows.push_back(obj);
            rowCount++;
        }
        QJsonDocument doc = QJsonDocument();
        if (header) {
            QJsonObject obj = QJsonObject();
            obj.insert("columns", cols);
            obj.insert("rows", rows);

            doc.setObject(obj);
        } else {
            doc.setArray(rows);
        }

        table = doc.toJson();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        error = QString(e.what());
    }

    return rowCount;

}

bool SqlInterface::insert(const QString &tableName, const QString &jsonObject, const QString& jsonObjectRef)
{
    auto doc = QJsonDocument::fromJson(jsonObject.toUtf8());
    auto obj = doc.object();
    QString query;
    QString into = "\n(";
    QString values_ = "\n(";
    QList<QVariant> values;

    query = "INSERT INTO ";
    query.append("dbo." + tableName);
    for (auto iter = obj.begin(); iter != obj.end(); iter++) {
        into.append(iter.key());
        if (iter != --obj.end())
            into.append(",\n");

        values_.append("?");
        if (iter != --obj.end())
            values_.append(",\n");

        if(iter.key() != "data")
            values.push_back(iter.value().toVariant());
        else{
            values.push_back(QByteArray::fromBase64(iter.value().toString().toUtf8()));
        }

    }
    into.append("\n)");
    values_.append("\n)");
    query.append(into);
    query.append("VALUES\n");
    query.append(values_);

    if(!jsonObjectRef.isEmpty()){
        auto docRef = QJsonDocument::fromJson(jsonObjectRef.toUtf8());
        auto ref = docRef.object();
        auto itr = ref.begin();
        QString query_ = QString("IF NOT EXISTS \n"
                                 "    (   SELECT  [%1]\n"
                                 "        FROM    dbo.%2 \n"
                                 "        WHERE   [%1]='%3' \n"
                                 "    )\n"
                                 "BEGIN\n").arg(itr.key(), tableName, itr.value().toString());
        query_.append(query);
        query_.append("\nEND");
        query = query_;
    }

    auto sql = QSqlQuery(getDatabase());
    sql.prepare(query);

    foreach(auto value , values){
        sql.addBindValue(value);
    }

    bool result = sql.exec();
    if(!result){
        std::cerr << sql.lastQuery().toStdString() << std::endl;
        std::cerr << sql.lastError().text().toStdString() << std::endl;
        return false;
    }

    return true;
}


bool SqlInterface::exportTablesToSqlServer()
{
    if(driver() != "QODBC")
        return false;

    auto dbSqlite = QSqlDatabase::addDatabase("QSQLITE","tempSqlite");
    QString file = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator() + "base/db.sqlite");
    QFile f(file);
    if(!f.exists())
    {
        std::cerr << "file: " << file.toStdString() << " not found!" << std::endl;
        return false;
    }
    std::cerr << "set sqlite database: " << file.toStdString() << std::endl;
    dbSqlite.setDatabaseName(file);

    if(!dbSqlite.open())
        return false;

    //QSqlDatabase::database().transaction();

    for (int itr = 0; itr < tables.size(); ++itr) {
        QString table = tables[itr];
        QString query = QString("select * from %1;").arg(table);
        auto result = dbSqlite.exec(query);
        if(result.lastError().type() == QSqlError::NoError){
            query = "INSERT INTO ";
            query.append("dbo.");
            query.append(table);
            QString into = "\n(";
            QString values_ = "\n(";
            int i = 0;
            int count = result.record().count() - 1;
            while (result.next()) {
                QString key = result.record().fieldName(i);
                if(key == "_id")
                    continue;
                if(i < count){
                    into.append(",\n");
                }
                QVariant value = result.value(i);
                if(value.typeId() == QMetaType::QString){
                    QString _value = value.toString();
                    values_.append("'" + _value + "'");
                    if (i < count)
                        values_.append(",\n");
                }else if(value.typeId() == QMetaType::Int){
                    QString _value = QString::number(value.toInt());
                    values_.append("'" + _value + "'");
                    if (i < count)
                        values_.append(",\n");
                }
                i++;
            }
            into.append("\n)");
            values_.append("\n)");
            query.append(into);
            query.append("\nVALUES\n");
            query.append(values_);
            query.append(";");

            QString err = "";

            exec(query, err);

            if (!err.isEmpty())
                std::cerr << "error export table: " << table.toStdString() << std::endl;
            else
                std::cout << "successful export table: " << table.toStdString() << std::endl;
        }else
           std::cerr << result.lastError().text().toStdString() <<std::endl;

    }

    dbSqlite.close();
    //QSqlDatabase::database().commit();
    QSqlDatabase::removeDatabase("tempSqlite");

    return true;
}

bool SqlInterface::verifyTables() {

    if(!isOpen())
        return false;

    //std::cout << db.databaseName().toStdString() << std::endl;
    //std::cout << db.hostName().toStdString() << std::endl;

    //bool result = false;
    for (int i = 0; i < tables.size(); ++i) {
        //result =
          verifyTable(i);
//        if (!result)
//            return false;
    }

    return true;
}


bool SqlInterface::insertSqlTableRow(const QString &table, QMap<QString, QVariant>& vals, const QString& ref)
{

    db.exec("USE arcirk;");

    QString query = queryText(table, vals, SqlCommand::sqlInsert, ref);

    QSqlQuery q = db.exec(query);

    bool result = q.lastError().type() == QSqlError::NoError;

    if(!result){
        qCritical() << __FUNCTION__ << qPrintable(query);
        qCritical() << q.lastError().text();
    }

    return result;
}

bool SqlInterface::updateSqlTableRow(const QString &table, QMap<QString, QVariant> vals, const QString& ref)
{

    QString query = queryText(table, vals, SqlCommand::sqlUpdate, ref);

    QSqlQuery q = db.exec(query);

    bool result = q.lastError().type() == QSqlError::NoError;

    if(!result){
        qCritical() << __FUNCTION__ << qPrintable(query);
        qCritical() << q.lastError().text();
    }

    return result;
}

bool SqlInterface::deleteSqlTableRow(const QString &table, const QString& ref)
{
    QMap<QString, QVariant> vals;
    QString query = queryText(table, vals, SqlCommand::sqlDelete, ref);

    QSqlQuery q = db.exec(query);

    bool result = q.lastError().type() == QSqlError::NoError;

    if(!result){
        qCritical() << __FUNCTION__ << qPrintable(query);
        qCritical() << q.lastError().text();
    }

    return result;
}

QString SqlInterface::queryText(const QString& table, QMap<QString, QVariant>& values,
                                SqlCommand command, const QString& not_ref)
{
    QString query;


    if(command == SqlCommand::sqlInsert){

        QString into = "\n(";
        QString values_ = "\n(";
        query = "INSERT INTO ";
        query.append("dbo." + table);
        for (auto iter = values.begin(); iter != values.end(); iter++) {
            into.append(iter.key());
            if (iter != --values.end())
                into.append(",\n");

            if (iter.value().typeId() == QMetaType::QString){
                QString value = iter.value().toString();
                values_.append("'" + value + "'");
                if (iter != --values.end())
                    values_.append(",\n");
            }else if (iter.value().typeId() == QMetaType::Int){
                int res = iter.value().toInt();
                QString value = QString::number(res);
                values_.append("'" + value + "'");
                if (iter != --values.end())
                    values_.append(",\n");
            }
        }
        into.append("\n)");
        values_.append("\n)");
        query.append(into);
        query.append("\nVALUES\n");
        query.append(values_);

        if(!not_ref.isEmpty()){
            QString query_ = QString("IF NOT EXISTS \n"
                                     "    (   SELECT  [Ref]\n"
                                     "        FROM    dbo.%1 \n"
                                     "        WHERE   [Ref]='%2' \n"
                                     "    )\n"
                                     "BEGIN\n").arg(table, not_ref);
            query_.append(query);
            query_.append("\nEND");
            query = query_;
        }

        query.append(";");

    }else if(command == SqlCommand::sqlUpdate){
        query = "UPDATE dbo." + table;
        QString _set = "\n SET ";
        QString _where = QString("\n WHERE Ref = '%1'").arg(not_ref);
        for (auto iter = values.begin(); iter != values.end(); iter++) {
            _set.append(iter.key());
            if (iter.value().typeId() == QMetaType::QString){
                QString value = iter.value().toString();
                _set.append(" = '" + value + "'");
                if (iter != --values.end())
                    _set.append(",\n");
            }else if (iter.value().typeId() == QMetaType::Int){
                int res = iter.value().toInt();
                QString value = QString::number(res);
                _set.append(" = '" + value + "'");
                if (iter != --values.end())
                    _set.append(",\n");
            }

        }
        _set.append("\n");
        _where.append("\n");
        query.append(_set);
        query.append(_where + ";");
    }else if(command == SqlCommand::sqlDelete){
        query = QString("DELETE FROM [dbo].[Servers]\n"
                "WHERE Ref = '%1'").arg(not_ref);
    }

    return query;

}

QString SqlInterface::lastError()
{
    return db.lastError().text();
}

QSqlDatabase SqlInterface::getDatabase() const
{
    return db;
}

bool SqlInterface::insert_file(const QString &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    auto obj = doc.object();

    QString query = obj.value("query").toString();
    if(query.isEmpty())
        return false;

    if(!db.isOpen())
        return false;

    auto _query = QSqlQuery();
    _query.prepare(query);

    auto values = obj.value("vaues").toArray();
    if(values.size() == 0)
        return false;

//    for(auto val : values){
//        if(val.toVariant().typeId() == QMetaType::QBitArray){
//            _query.addBindValue(val.is);
//        }
    //    }

    return false;
}

QStringList SqlInterface::getParametersFromString(const QString &str)
{
    QStringList result;
    QRegularExpression re("\\[(.*?)\\]");
    auto it = re.globalMatch( str );
    while( it.hasNext() ) {
        auto match = it.next();
        //qDebug() << match.captured( 0 ) << ":" << match.captured( 1 );
        result.append( match.captured( 1 ));
    }
    return result;

}

