#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QJsonObject>

class appSettings{
public:
    appSettings();

    bool init();
    void save_settings();
    QString getJson();
    QJsonObject getJsonObject();

    QString ServerHost;
    int ServerPort;
    QString RootUser;
    QString Hash;
    QString ServerBinDir;
    QString ServerName;
    QString ServerStatus;
    bool useWebDav;
    QString davUser;
    QString davPassword;
    QString localFolder;
    QString webDavAddress;
};

#endif // APPSETTINGS_H
