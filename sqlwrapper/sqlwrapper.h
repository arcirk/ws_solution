#ifndef SQLWRAPPER_H
#define SQLWRAPPER_H

#include "sqlwrapper_global.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

    void setHost(char const* value);
    [[nodiscard]] std::string host() const;
    void setSqlUser(char const* value);
    [[nodiscard]] std::string sqlUser() const;
    void setSqlPwd(char const* value);
    [[nodiscard]] std::string sqlPwd() const;
    void setDatabaseName(char const* value);
    [[nodiscard]] std::string databaseName() const;

    bool connect(char const* dbName, char const* usr = "", char const* pass = "", DriverType driverType = sqlserver);
    bool verifyTables();
    bool verifyViews();
    bool isOpen();

    int exec(char const* query, char* err);
    int execute(char const* query, char** result, char** err, bool header = false);
    int execute(const std::string &query, std::string &result, std::string &err, bool header = false);

private:
    SqlInterface * sql;
    std::string _host;
    std::string _user;
    std::string _pwd;
    std::string _databaseName;
    DriverType _driverType;

};

#endif // SQLWRAPPER_H
