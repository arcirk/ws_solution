#ifndef SQLWRAPPER_H
#define SQLWRAPPER_H

#include "sqlwrapper_global.h"

#include <iostream>

class SqlInterface;


enum DriverType{
    sqlserver = -1,
    sqlite
};

class SQLWRAPPER_EXPORT SqlWrapper
{
public:

    SqlWrapper();
    ~SqlWrapper();

    void setHost(const std::string& value);
    std::string host() const;
    void setSqlUser(const std::string& value);
    std::string sqlUser() const;
    void setSqlPwd(const std::string& value);
    std::string sqlPwd() const;
    void setDatabaseName(const std::string& value);
    std::string databaseName() const;

    bool connect(const std::string& dbName, const std::string& usr = "", const std::string& pass = "", DriverType driverType = sqlserver);
    bool isOpen();

private:
    SqlInterface * sql;
    std::string _host;
    std::string _user;
    std::string _pwd;
    std::string _databaseName;
    DriverType _driverType;


};

#endif // SQLWRAPPER_H
