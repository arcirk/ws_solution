#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QJsonObject>

class ClientSettings{

public:
    ClientSettings();

    ClientSettings &operator=(const ClientSettings& from);

    bool init();
    void save_settings();
    QString getJson();
    QJsonObject getJsonObject();

    QString ServerHost;
    int ServerPort;
    QString RootUser;
    QString Hash;
    QString ServerName;   
    QString AppName;
    bool AutoConnect;
    bool SaveHash;
    QString ServerWorkingDirectory;
    bool isLocalInstallation;
    bool RunAsService;
    bool UseLocalWebDAvDirectory;
    QString LocalWebDavDirectory;
    QString WebdavHost;
    QString WebdavUser;
    QString WebdavPwd;
    bool WebdavSSL;

    QString password;
    bool pwdEdit;

    void setSettingsFileName(const QString& fname);
    bool installService(const QString &pwd);
    bool startService(const QString &pwd);
    bool stopService(const QString &pwd);
    bool statusService();


private:
    QString fileName;
    QString ServerStatus;

    bool createDaemonFile();
    bool installDaemonFile(const QString &pwd);

};

#endif // APPSETTINGS_H
