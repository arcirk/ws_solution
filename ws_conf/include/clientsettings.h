#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QJsonObject>

class ClientSettings{

public:
    ClientSettings();

    bool init();
    void save_settings();
    QString getJson();
    QJsonObject getJsonObject();

    QString Host;
    int Port;
    QString User;
    QString Hash;
    QString ServerBinaryDir;
    QString ServerName;
    QString Target;
    bool AutoConnect;
    bool SaveHash;
    QString ClientBinaryDir;
    QString LocalWebDavDirectory;
    bool UseLocalWebDavDirectory;
    QString WebdavHost;
    QString WebdavUser;
    QString WebdavPwd;
    bool WebdavSSL;

};

#endif // APPSETTINGS_H
