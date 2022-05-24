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

SqlInterface::SqlInterface()
{

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
        db = QSqlDatabase::addDatabase(driver,"default");

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

        //QSqlDatabase::removeDatabase("default");

        result = db.open();


    }catch (std::exception& e){
        std::cerr << e.what() << std::endl;

    }
    return result;
}

bool SqlInterface::isOpen()
{
    return db.isOpen();
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
        std::cout << QString("Connected database '%1'").arg(databaseName()).toStdString() << std::endl;
        return true;
    }

    return false;

}

int SqlInterface::exec(const QString &query, QString& err) {

    err = "";
    int count = 0;
    try{
        if(!isOpen())
            return 0;

        auto result = db.exec(query);
        if(result.lastError().isValid()){
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

bool SqlInterface::verifyTable(int tableIndex) {

    if(!isOpen() || _driverType != "QODBC" || databaseName().isEmpty())
        return false;

    QString vTable = QString("SELECT * FROM sysobjects where  name = '%1'").arg(tables[tableIndex]);
    auto sqlQuery = db.exec(vTable);
    bool isExists = false;
    while (sqlQuery.next()){
        isExists = true;
        break;
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
                    "[FirstField] [char](150) NULL,\n"
                    "[SecondField] [char](150) NULL,\n"
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
                 "[Performance] [char](150) NULL,\n"
                 "[channel] [char](36) NOT NULL,\n"
                 "[parent] [char](36) NULL,\n"
                 "[cache] [text] NULL,";
    }else if(tableName == "Channels"){
        result = "[parent] [char](36) NOT NULL,\n"
                 "[cache] [text] NULL,";
    }else if(tableName == "Messages"){
        result = "[message] [text] NULL,\n"
                 "[token] [char](255) NOT NULL,\n"
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
                 "[data] [varbinary](max) NULL,";
    }else if(tableName == "CertUsers"){
        result = "[cache] [text] NULL,\n"
                 "[uuid] [char](38) NULL,\n"
                 "[sid] [varchar](136) NULL,\n"
                 "[domain] [char](100) NULL,";
    }


    return result;
}

void SqlInterface::setIndexes(int tableIndex) {

    const QString& tableName = tables[tableIndex];
    if(tableName == "Users"){
        db.exec("ALTER TABLE [dbo].[Users] ADD  CONSTRAINT [DF_Users_channel]  DEFAULT (((((0)-(0))-(0))-(0))-(0)) FOR [channel]");
    }else if(tableName == "Channels"){
        db.exec("ALTER TABLE [dbo].[Channels] ADD  CONSTRAINT [DF_Channels_Parent]  DEFAULT (((((0)-(0))-(0))-(0))-(0.)) FOR [Parent]");
    }else if(tableName == "Messages"){
        db.exec("ALTER TABLE [dbo].[Messages] ADD  CONSTRAINT [DF_Messages_contentType]  DEFAULT ('HTML') FOR [contentType]");
        db.exec("ALTER TABLE [dbo].[Messages] ADD  CONSTRAINT [DF_Messages_unreadMessages]  DEFAULT ((0)) FOR [unreadMessages]");
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

        QSqlQuery m_query;// = QSqlQuery(QString::fromStdString(query), db);
        bool result = false;
        try {
            //result = m_query.exec();
            m_query = db.exec(QString::fromStdString(query));
            result = true;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }

        if(!result){
            if (m_query.lastError().isValid()) {
                error = m_query.lastError().text();
                std::cerr << "SqlInterface::execute: " << query << std::endl;
                std::cerr << "SqlInterface::execute: " << "Ошибка SQL запроса : " << error.toStdString() << std::endl;
                std::cerr << m_query.lastError().databaseText().toStdString() << std::endl;
                std::cerr << m_query.lastError().driverText().toStdString() << std::endl;
                std::cerr << m_query.lastQuery().toStdString() << std::endl;
            }
            return 0;
        }

        int count = m_query.record().count();

        auto rows = QJsonArray();
        auto cols = QJsonArray();

        while (m_query.next()) {
            QJsonObject obj = QJsonObject();
            for (int i = 0; i < count; ++i) {
                QString key = m_query.record().fieldName(i);
                cols.push_back(key);
                QVariant val = m_query.value(i);
                if (val.typeId() == QMetaType::QString)
                    obj.insert(key, val.toString().trimmed());
                else if (val.typeId() == QMetaType::Double)
                    obj.insert(key, val.toDouble());
                else if (val.typeId() == QMetaType::Int)
                    obj.insert(key, val.toInt());
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
    }

    return rowCount;

}

bool SqlInterface::exportTablesToSqlServer()
{
    if(driver() != "QODBC")
        return false;

    auto dbSqlite = QSqlDatabase::addDatabase("QSQLITE","tempSqlite");
    QString file = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator() + "db.sqlite");
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
    QSqlDatabase::removeDatabase("tempSqlite");

    return true;
}

bool SqlInterface::verifyTables() {

    if(!isOpen())
        return false;

    bool result = false;
    for (int i = 0; i < tables.size(); ++i) {
        result = verifyTable(i);
        if (!result)
            return false;
    }

    return true;
}
