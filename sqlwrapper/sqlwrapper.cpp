#include "sqlwrapper.h"
#include "sqlinterface.h"
#include <QUuid>

SqlWrapper::SqlWrapper()
{
    sql = new SqlInterface();
}

SqlWrapper::~SqlWrapper()
{
    delete sql;
}

void SqlWrapper::setHost(char const* value)
{
    _host = std::string(value);
    sql->setHost(QString::fromStdString(value));
}

std::string SqlWrapper::host() const
{
    return _host;
}

void SqlWrapper::setSqlUser(char const* value)
{
    _user = std::string(value);
    sql->setSqlUser(QString::fromStdString(value));
}

std::string SqlWrapper::sqlUser() const
{
    return _user;
}

void SqlWrapper::setSqlPwd(char const* value)
{
    _pwd = std::string(value);
    sql->setSqlPwd(QString::fromStdString(value));
}

std::string SqlWrapper::sqlPwd() const
{
    return _pwd;
}

void SqlWrapper::setDatabaseName(char const* value)
{
    _databaseName = std::string(value);
    sql->setDatabaseName(QString::fromStdString(value));
}

std::string SqlWrapper::databaseName() const
{
    return _databaseName;
}

bool SqlWrapper::connect(char const* dbName, char const* usr, char const* pass, DriverType driverType)
{

    if(!usr)
        setSqlUser(usr);
    if(!pass)
        setSqlPwd(pass);

    setDatabaseName(dbName);

    QString drv = driverType ? "QODBC" : "QSQLITE";

    _driverType = driverType;

    bool result = sql->connect(drv);

    if(result && driverType == DriverType::sqlserver){
        result = sql->verifyDatabase();
    }

    return result;
}

bool SqlWrapper::isOpen()
{
    return sql->isOpen();
}

int SqlWrapper::exec(char const* query, char* err) {
    if(!sql->isOpen()) {
        strcpy(err, "Database not open!");
        return 0;
    }
    QString error;
    int result = sql->exec(query, error);
    strcpy(err, error.toStdString().c_str());
    return result;
}

bool SqlWrapper::verifyTables() {

    if(!sql->isOpen())
        return false;

    bool result = false;
    for (int i = 0; i < tables.size(); ++i) {
        result = sql->verifyTable(i);
        if (!result)
            return false;
    }

    return true;
}

bool SqlWrapper::verifyViews() {
    return sql->verifyViews();
}

//void SqlWrapper::insert(char const* table, char  const* values) {
//
//    QString err;
//    bool result = sql->insert(table, values, err);
//
//}

int
SqlWrapper::execute(char const* query, char* result, char* err, bool header){

    if(!sql->isOpen()) {
        strcpy(err, "Database not open!");
        return 0;
    }
    QString _result;
    QString error;
    int res = sql->execute(query, _result, error, header);
    strcpy(err, error.toStdString().c_str());
    strcpy(result, _result.toStdString().c_str());
    return res;
}
//
//bool SqlWrapper::save_message(const char *message, const char * first,
//                                const char * second, char *ref, bool active,
//                                const char *firstName) {
//
//    if(!sql->isOpen())
//        return false;
//    QString _ref;
//    bool result = sql->save_message(message, first, second,
//                                    _ref, active, firstName);
//    strcpy(ref, _ref.toStdString().c_str());
//    return result;
//}
//
//void SqlWrapper::get_channel_token(const char *first, const char *second, char *result) {
//
//    QString res;
//
//    sql->get_channel_token(first, second, res);
//
//    strcpy(result, res.toStdString().c_str());
//
//}
