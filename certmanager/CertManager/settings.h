#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QJsonObject>

enum launchMode{
    ws_srv = 0,
    mixed
};

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr, const QString& parentFolder = "");

    void setUser(const QString& usr);
    void setServer(const QString& val);
    void setPwd(const QString& val);
    void setLanchMode(launchMode value);

    void setHttpHost(const QString& val);
    void setHttpUsr(const QString& val);
    void setHttpPwd(const QString& val);

    launchMode launch_mode();
    QString user() const;
    QString server() const;
    QString pwd() const;

    QString httpHost() const;
    QString httpUsr() const;
    QString httpPwd() const;

    bool useSettingsFromHttp();
    void setUseSettingsFromHttp(bool value);

    bool customWsUser();
    void setCustomWsUser(bool value);

    void save();
    QJsonObject to_object();

    QString charset();
    void setCharset(const QString& cp);

    int method();
    void setMethod(int m);

private:
    const QString connectionStringTemplate = "DRIVER={SQL Server};"
                                             "SERVER=%1;DATABASE=%2;Persist Security Info=true;"
                                             "uid=%3;pwd=%4";
    const QString databaseName = "arcirk";

    QString _user;
    QString _pwd;
    QString _server;
    int _method;
    QString _charset;
    launchMode _launch_mode;
    QString _parentFolder;

    QString _httpHost;
    QString _httpUsr;
    QString _httpPwd;
    bool _useSettingsFromHttp;
    bool _customWsUser;

signals:

};

#endif // SETTINGS_H
