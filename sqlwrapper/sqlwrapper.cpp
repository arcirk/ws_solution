#include "sqlwrapper.h"
#include "sqlinterface.h"


SqlWrapper::SqlWrapper()
{
    sql = new SqlInterface();
}

SqlWrapper::~SqlWrapper()
{
    delete sql;
}

void SqlWrapper::setHost(const std::string &value)
{
    _host = value;
    sql->setHost(QString::fromStdString(value));
}

std::string SqlWrapper::host() const
{
    return _host;
}

void SqlWrapper::setSqlUser(const std::string &value)
{
    _user = value;
    sql->setSqlUser(QString::fromStdString(value));
}

std::string SqlWrapper::sqlUser() const
{
    return _user;
}

void SqlWrapper::setSqlPwd(const std::string &value)
{
    _pwd = value;
    sql->setSqlPwd(QString::fromStdString(value));
}

std::string SqlWrapper::sqlPwd() const
{
    return _pwd;
}

void SqlWrapper::setDatabaseName(const std::string &value)
{
    _databaseName = value;
    sql->setDatabaseName(QString::fromStdString(value));
}

std::string SqlWrapper::databaseName() const
{
    return _databaseName;
}

bool SqlWrapper::connect(const std::string& dbName, const std::string &usr, const std::string &pass, DriverType driverType)
{
    setSqlUser(usr);
    setSqlPwd(pass);
    setDatabaseName(dbName);

    _driverType = driverType;

    QString drv = driverType ? "QODBC" : "QSQLITE";

    return sql->connect(drv);
}

bool SqlWrapper::isOpen()
{
    return sql->isOpen();
}
