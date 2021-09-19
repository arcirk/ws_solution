#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QJsonObject>

class appSettings{
public:
    appSettings();

    bool init();
    void save_settings();

    QString ServerHost;
    int ServerPort;
    QString RootUser;
    QString Hash;
    QString ServerBinDir;

};

#endif // APPSETTINGS_H
