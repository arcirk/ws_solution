#ifndef _SQLWRAPPER_H
#define _SQLWRAPPER_H

#include <iostream>

enum DriverType{
    sqlserver = -1,
    sqlite
};

class SqlWrapper
{
public:

    SqlWrapper();
    ~SqlWrapper();

    void setHost(const std::string& value);
    [[nodiscard]] std::string host() const;
    void setSqlUser(const std::string& value);
    [[nodiscard]] std::string sqlUser() const;
    void setSqlPwd(const std::string& value);
    [[nodiscard]] std::string sqlPwd() const;
    void setDatabaseName(const std::string& value);
    [[nodiscard]] std::string databaseName() const;

    bool connect(const std::string& dbName, const std::string& usr = "", const std::string& pass = "", enum DriverType driverType = sqlserver);
    bool isOpen();

private:
    std::string _host;
    std::string _user;
    std::string _pwd;
    std::string _databaseName;
    enum DriverType _driverType;


};
#endif // _SQLWRAPPER_H
