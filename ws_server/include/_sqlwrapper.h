#ifndef _SQLWRAPPER_H
#define _SQLWRAPPER_H

#define SQL_WRAPPER_EXPORT   __declspec( dllexport )

#include <iostream>

enum DriverType{
    sqlserver = -1,
    sqlite
};

class SQL_WRAPPER_EXPORT SqlWrapper
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
    bool connect(char const* dbName, char const* usr = "", char const* pass = "", enum DriverType driverType = sqlserver);
    bool isOpen();
    bool verifyTables();
    bool verifyViews();

    int exec(char const* query, char* err);
    int execute(char const* query, char* result, char* err, bool header = false);


};
#endif // _SQLWRAPPER_H
