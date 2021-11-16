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

    QString ServerHost;
    int ServerPort;
    QString RootUser;
    QString Hash;
    QString ServerBinDir;
    QString ServerName;
    QString ServerStatus;
    QString AppName;
    bool AutoConnect;
    bool SaveHash;
};

#endif // APPSETTINGS_H
